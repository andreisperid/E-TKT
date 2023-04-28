// MIT License

// Copyright (c) 2022 Andrei Speridião

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

//
// for more information, please visit https://github.com/andreisperid/E-TKT
//

let busy = false;
let align;
let force;
let alignTemp;
let forceTemp;
let scrollbarHeight = 0;

window.onload = startupRoutine;

async function startupRoutine() {
  checkOverlayScrollbars();
  let body = document.getElementsByTagName("body")[0];
  body.dataset.printing = "false";
  drawHelper();
  document.getElementById("text-input").focus();
  await retrieveSettings();
  await getStatus();
}

function checkScrollbarWidth() {
  const outer = document.createElement("div");
  outer.style.overflow = "scroll";
  document.body.appendChild(outer);

  const scrollbarWidth = outer.offsetWidth - outer.clientWidth;
  document.body.removeChild(outer);

  return scrollbarWidth;
}

/**
 * Attempts to detect if overlay scrollbars are in use, and adds a css class we can
 * change layout behavior on.
 */
function checkOverlayScrollbars() {
  const scrollbarWidth = checkScrollbarWidth();
  if (scrollbarWidth === 0) {
    document.body.classList.add("overlay-scroll-enabled");
  }
}

async function retrieveSettings() {
  // TODO: consolidate this method with getStatus(), since they both now use the same API method.

  // retrieve settings from the device
  let request = await fetchWithTimeout("api/status", { timeout: 5000 });
  let response = await request.json();

  align = response.align;
  force = response.force;
  alignTemp = align;
  forceTemp = force;

  document.getElementById("align-field").value = align;
  document.getElementById("force-field").value = force;
}

function measureText(element, text) {
  // Create a temporary canvas element
  const canvas = document.createElement("canvas");
  const context = canvas.getContext("2d");

  // Apply the styles (height and font) from the element to the context
  const style = getComputedStyle(element);
  context.font = `${style.fontSize} ${style.fontFamily}`;

  // Measure the text
  const metrics = context.measureText(text);

  // Return the width
  return metrics.width;
}

function calculateLength() {
  // calculates the label length based on the number of characters (including spaces)

  let label = document.getElementById("length-label");
  let treatedLabel = buildTreatedLabel();

  if (!isValidLabelText()) {
    label.innerHTML = "??mm";
    label.style.opacity = 0.2;
  } else {
    label.innerHTML = (treatedLabel.length < 7 ? 7 : treatedLabel.length) * 4 + "mm";
    label.style.opacity = 1;
  }
}

async function labelCommand() {
  // sends the label to the device
  if (isValidLabelText()) {
    document.getElementById("text-input").blur();
    setUiBusy(true);
    let response = await postJson("api/task", { parameter: "tag", value: buildTreatedLabel().toLowerCase() });
    if (!response.ok) {
      console.error("Unable to feed");
      console.error((await response.json())["error"]);
    }
  }
}

function buildTreatedLabel() {
  const LabelInput = document.getElementById("text-input");
  let fieldValue = LabelInput.value;
  if (fieldValue.length == 0) {
    fieldValue = "WRITE HERE";
  }
  switch (document.getElementById("mode-dropdown").value) {
    case "tight":
      multiplier = 0;
      break;
    default:
      multiplier = 1;
      break;
  }

  const printLength = fieldValue.length + multiplier * 2;
  if (printLength < 7) {
    multiplier = Math.ceil((7 - printLength) / 2);
  }
  return " ".repeat(multiplier) + fieldValue + " ".repeat(multiplier);
}

function getScrollbarHeight(element) {
  if (element.scrollHeight > element.clientHeight) {
    return element.offsetHeight - element.clientHeight;
  } else {
    return 0;
  }
}

function drawHelper() {
  // draws visual helper with label length taking options into account
  const labelInput = document.getElementById("text-input");
  const labelText = labelInput.value;
  const scroll = document.getElementById("text-form-scroll"); // picks up the parent scroll element
  const border = document.getElementById("text-form-border");

  document.getElementById("clear-button").disabled = labelText === "";
  document.getElementById("submit-button").disabled = labelText === "";
  document.getElementById("reel-button").disabled = !(labelText === "");
  document.getElementById("feed-button").disabled = !(labelText === "");
  document.getElementById("cut-button").disabled = !(labelText === "");
  document.getElementById("setup-button").disabled = !(labelText === "");

  labelInput.style.width = measureText(labelInput, buildTreatedLabel()) + 4 + "px";
  labelInput.style.minWidth = measureText(labelInput, " ".repeat(7)) + 4 + "px";

  const neededWidth = labelInput.clientWidth + 4;

  if (neededWidth > scroll.clientWidth) {
    // If modifying the text near the beginning or end of the scrollable area, then
    // move the scroll area to keep the border visible while editing for better context.
    if (labelInput.selectionEnd && labelInput.selectionEnd >= labelText.length - 20) {
      scroll.scrollLeft = scroll.scrollWidth - scroll.clientWidth;
    } else if (labelInput.selectionStart && labelInput.selectionStart < 20) {
      scroll.scrollLeft = 0;
    }
    // Avoid leaving the scroll position past the end of the "needed" scrollable area.
    // Dunno why browsers let you do this.
    if (scroll.scrollLeft + neededWidth > scroll.scrollWidth) {
      scroll.scrollLeft = neededWidth - scroll.clientWidth;
    }
    border.classList.add("scrolling");
  } else {
    border.classList.remove("scrolling");
    scroll.scrollLeft = 0;
  }

  onTextInputSelectionchange();
}

function onTextInputSelectionchange() {
  const labelInput = document.getElementById("text-input");
  const scroll = document.getElementById("text-form-scroll");

  // If the selection is at the beginning or end of the text, then move the scroll area to
  // the beginning or end to include the label margins.
  if (labelInput.selectionStart == labelInput.selectionEnd && labelInput.selectionEnd == 0) {
    scroll.scrollLeft = 0;
  } else if (
    labelInput.selectionStart == labelInput.selectionEnd &&
    labelInput.selectionEnd == labelInput.value.length
  ) {
    scroll.scrollLeft = scroll.scrollWidth - scroll.clientWidth;
  }
}

function isValidLabelText() {
  // test for suported characters
  // $-.23456789*abcdefghijklmnopqrstuvwxyz♡☆♪€@

  //  supported emoji:
  // 	♡
  // 	☆
  // 	♪
  // 	€
  // 	@
  // 	$
  let labelInput = document.getElementById("text-input");
  let regex = /^[a-zA-Z0-9 .\-♡☆♪€@$]+$/i;
  return labelInput.value.length > 0 && regex.test(labelInput.value);
}

function updateScrollHelper() {
  // shows "..." helper if text is overflowed to that side

  let scroll = document.getElementById("text-form-scroll");
  let leftHelper = document.getElementById("tip-left");
  let rightHelper = document.getElementById("tip-right");

  // console.log(Math.round(scroll.scrollLeft + scroll.offsetWidth), scroll.scrollWidth);

  if (scroll.scrollWidth > scroll.offsetWidth) {
    if (
      Math.round(scroll.scrollLeft + scroll.offsetWidth) >=
      scroll.scrollWidth - 1 // "1" is margin of error
    ) {
      rightHelper.classList.remove("visible");
    } else {
      rightHelper.classList.add("visible");
    }
    if (scroll.scrollLeft == 0) {
      leftHelper.classList.remove("visible");
    } else {
      leftHelper.classList.add("visible");
    }
  } else {
    leftHelper.classList.remove("visible");
    rightHelper.classList.remove("visible");
  }
}

function jumpToScrollEnds(target) {
  // jumps to the scroll target where 0 is the start and 1 the end

  let labelInput = document.getElementById("text-input");
  labelInput.focus();
  labelInput.setSelectionRange(target * labelInput.value.length, target * labelInput.value.length );

  // let scroll = document.getElementById("text-form-scroll");
  // scroll.scrollTo({ left: target * scroll.scrollWidth, behavior: "smooth" });
}

function lerp(start, end, amt) {
  return (1 - amt) * start + amt * end;
}

function validateField() {
  // instantly validates label field by blocking buttons and giving visual feedback
  let labelInput = document.getElementById("text-input");
  drawHelper();

  if (!isValidLabelText() && labelInput.value != "") {
    document.getElementById("hint").style.color = "red";
    document.getElementById("text-input").style.color = "red";
    document.getElementById("submit-button").disabled = true;
    document.getElementById("submit-button").value = " invalid entry ";
    document.getElementById("submit-button").style.color = "red";
  } else {
    document.getElementById("hint").style.color = "#e7dac960";
    document.getElementById("text-input").style.color = "#e7dac9ff";
    document.getElementById("submit-button").value = labelInput.value != "" ? " Print label! " : " ... ";
    document.getElementById("submit-button").style.color = "#e7dac9ff";
  }
}

function labelTextChanged() {
  validateField();
  calculateLength();
  drawHelper();
}

function labelTextKeyDown(e) {
  if (e.key === "Enter" && isValidLabelText()) {
    document.getElementById("submit-button").click();
  }
  onTextInputSelectionchange();
}

function marginDropdownChanged(e) {
  validateField();
  calculateLength();
  drawHelper();
}

function clearField() {
  // clears the label field and restore default button and form states
  const labelInput = document.getElementById("text-input");

  document.getElementById("clear-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("reel-button").disabled = false;
  document.getElementById("feed-button").disabled = false;
  document.getElementById("cut-button").disabled = false;
  document.getElementById("hint").style.color = "#777777";
  document.getElementById("text-input").style.color = "#ffffff";
  document.getElementById("submit-button").value = " ... ";

  labelInput.value = "";

  drawHelper();
  calculateLength();

  labelInput.focus();
}

function updateTempValues() {
  // updates the temporary setting values

  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;
}

function changeField(action, fieldName) {
  // incremental / decremental buttons for the align and force settings

  const field = document.getElementById(fieldName);
  let currentValue = 0;
  currentValue = field.value;

  if (action == "add" && currentValue + 1 <= field.max) {
    currentValue++;
    field.value = currentValue;
  } else if (action == "remove" && currentValue - 1 >= field.min) {
    currentValue--;
    field.value = currentValue;
  }
  updateTempValues();
}

function insertIntoField(specialChar) {
  // inserts special emoji character in the label form

  const labelInput = document.getElementById("text-input");
  labelInput.focus();

  let insertStartPoint;
  let insertEndPoint;
  let value = labelInput.value;

  if (labelInput.selectionStart == labelInput.selectionEnd) {
    insertStartPoint = labelInput.selectionStart;
    insertEndPoint = insertStartPoint;
  } else {
    insertStartPoint = labelInput.selectionStart;
    insertEndPoint = labelInput.selectionEnd;
  }

  // text before cursor/highlighted text + special character + text after cursor/highlighted text
  value = value.slice(0, insertStartPoint) + specialChar + value.slice(insertEndPoint);
  labelInput.value = value;

  labelInput.setSelectionRange(insertStartPoint + 1, insertStartPoint + 1);
  validateField();
  labelInput.focus();
}

function validateText(input) {
  // test for suported characters
  // $-.23456789*abcdefghijklmnopqrstuvwxyz♡☆♪€@

  //  supported emoji:
  // 	♡
  // 	☆
  // 	♪
  // 	€
  // 	@
  // 	$

  let regex = /^[a-zA-Z0-9 .\-♡☆♪€@$]+$/i;
  return regex.test(input);
}

function toggleSettings(safe = true) {
  // shows/hide settings page

  let state = document.getElementById("settings-frame").style.visibility;

  // console.log(state);
  // console.log(align + " / " + alignTemp + " / / " + force + " / " + forceTemp);

  if (state === "hidden") {
    retrieveSettings();
    alignTemp = align;
    forceTemp = force;
    document.getElementById("settings-frame").style.visibility = "visible";
    document.getElementById("main-frame").style.visibility = "hidden";
  } else {
    if (!safe || (align == alignTemp && force == forceTemp) || confirm("Discard unsaved changes?")) {
      document.getElementById("settings-frame").style.visibility = "hidden";
      document.getElementById("main-frame").style.visibility = "visible";
      alignTemp = align;
      forceTemp = force;
    }
  }
}

async function reelCommand() {
  // sends reel command to the device
  let prompt = confirm(
    "Confirm loading a new reel?\n\nPlease make sure the tape is touching the cog.\n\nImportant: unsaved align and force settings will be lost."
  );
  if (prompt) {
    toggleSettings(false);
    setUiBusy(true);
    document.getElementById("submit-button").value = " reeling... ";
    let response = await postJson("api/task", { parameter: "reel", value: "" });
    if (!response.ok) {
      console.error("Unable to reel");
      console.error((await response.json())["error"]);
    }
  }
}

async function feedCommand() {
  // sends feed command to the device
  setUiBusy(true);
  document.getElementById("submit-button").value = " feeding... ";
  let response = await postJson("api/task", { parameter: "feed", value: "" });
  if (!response.ok) {
    console.error("Unable to feed");
    console.error((await response.json())["error"]);
  }
}

async function cutCommand() {
  // sends cut command to the device
  setUiBusy(true);
  document.getElementById("submit-button").value = " cutting... ";
  let response = await postJson("api/task", { parameter: "cut", value: "" });
  if (!response.ok) {
    console.error("Unable to cut");
    console.error((await response.json())["error"]);
  }
}

async function testCommand(testFull) {
  // sends test command to the device
  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;
  let data;
  if (testFull) {
    data = {
      parameter: "testfull",
      value: align + "," + force,
    };
  } else {
    data = {
      parameter: "testalign",
      value: align + "," + 1,
    };
  }
  setUiBusy(true);
  let response = await postJson("api/task", data);
  if (!response.ok) {
    console.error("Unable to perform test");
    console.error((await response.json())["error"]);
  }
}

async function settingsCommand() {
  // sends settings save command to the device, and triggers self restart in 15 seconds

  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;

  // console.log("settings / align (" + align + ") force (" + force + ")");

  if (confirm("Confirm saving align [" + align + "] and force [" + force + "] settings?")) {
    setUiBusy(true);
    document.getElementById("submit-button").value = " saving... ";
    let response = await postJson("api/task", { parameter: "save", value: align + "," + force });
    if (!response.ok) {
      console.error("Unable to save settings");
      console.error((await response.json())["error"]);
      return;
    }

    document.getElementById("settings-frame").style.visibility = "hidden";
    document.getElementById("refresh-frame").style.visibility = "visible";

    let count = 15;
    document.getElementById("countdown").textContent = count;

    setInterval(function () {
      count = count - 1;
      document.getElementById("countdown").textContent = count;

      // console.log(count);

      if (count == 0) {
        window.location.reload();
      }
    }, 1000);
  }
}

// Helper method to amke fetch requests with a configurable timeout.
// See: https://dmitripavlutin.com/timeout-fetch-request/
async function fetchWithTimeout(resource, options = {}) {
  const { timeout = 8000 } = options;

  const controller = new AbortController();
  const id = setTimeout(() => controller.abort(), timeout);
  const response = await fetch(resource, {
    ...options,
    signal: controller.signal,
  });
  clearTimeout(id);
  return response;
}

// Helper method to post a json request, supports timeouts.
async function postJson(url, data, options = {}) {
  options.headers = {
    "Content-Type": "application/json",
    Accept: "application/json",
  };
  options.body = JSON.stringify(data);
  options.method = "POST";
  return await fetchWithTimeout(url, options);
}

async function getStatus() {
  try {
    let request = await fetchWithTimeout("api/status", { timeout: 5000 });
    handleData(await request.json());
  } catch (error) {
    // TODO: Add some UI treatment for when there are communication errors, eg
    // a "Reconnecting..." toast message or something.
    console.error("Problem ");
    console.error(error);
  } finally {
    setTimeout(getStatus, 1000);
  }
}

wasBusy = false;

// Enables or disables UI elements to prevent intercations while the printer is printing,
// reeling, cutting, etc.
function setUiBusy(busy) {
  if (busy && !wasBusy) {
    // Disable UI elements
    wasBusy = true;
    Array.from(document.querySelectorAll("input")).forEach((element) => {
      element.disabled = true;
    });
    document.getElementById("mode-dropdown").disabled = true;
  } else if (!busy && wasBusy) {
    // Enable UI elements
    wasBusy = false;
    let body = document.getElementsByTagName("body")[0];
    body.dataset.printing = "false";
    const labelInput = document.getElementById("text-input");
    Array.from(document.querySelectorAll("input")).forEach((element) => {
      element.disabled = false;
    });
    document.getElementById("mode-dropdown").disabled = false;
    document.getElementById("submit-button").disabled = labelInput.value == "";
    document.getElementById("clear-button").disabled = labelInput.value == "";
    validateField();
  }
}

function handleData(data_json) {
  setUiBusy(data_json.busy);

  if (!data_json.busy) {
    return;
  }
  let percentage = parseInt(data_json.progress);
  if (percentage > 0) {
    percentage -= 1; // avoid 100% progress while still finishing
  }

  let scroll = document.getElementById("text-form-scroll"); // picks up the parent scroll element

  switch (data_json.command) {
    case "tag":
      document.getElementById("submit-button").value = " printing " + percentage + "% ";
      let body = document.getElementsByTagName("body")[0];
      body.dataset.printing = "true";
      const label = buildTreatedLabel();
      const labelInput = document.getElementById("text-input");
      const printed = label.substring(0, Math.round(label.length * (percentage / 100)));
      const progressLength = measureText(labelInput, printed);
      document.getElementById("progress-bar").style.width = progressLength + "px";

      if (progressLength < scroll.clientWidth / 2) {
        scroll.scrollLeft = 0;
      } else if (progressLength > scroll.scrollWidth - scroll.clientWidth / 2) {
        scroll.scrollLeft = scroll.scrollWidth;
      } else {
        scroll.scrollLeft = progressLength - scroll.clientWidth / 2;
      }
      break;
    case "reel":
      document.getElementById("submit-button").value = " reeling... ";
      break;
    case "feed":
      document.getElementById("submit-button").value = " feeding... ";
      break;
    case "cut":
      document.getElementById("submit-button").value = " cutting... ";
      break;
  }
}
