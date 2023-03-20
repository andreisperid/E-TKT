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
let command = "";
let treatedLabel = "";
let clear = false;
let minSize = 7;
let caretPosition = 0;
let align;
let force;
let alignTemp;
let forceTemp;

window.onload = startupRoutine;

async function startupRoutine() {
  document.getElementById("text-input").focus();
  await retrieveSettings();
  await getStatus();
}

async function retrieveSettings() {
  // TODO: consolidate this method with getStatus(), since they both now use the same API method.

  // retrieve settings from the device
  let request = await fetchWithTimeout("api/status", {timeout: 5000});
  let response = await request.json();

  align = response.align;
  force = response.force;
  alignTemp = align;
  forceTemp = force;

  document.getElementById("align-field").value = align;
  document.getElementById("force-field").value = force;
}

function calculateLength() {
  // calculates the label length based on the number of characters (including spaces)

  let label = document.getElementById("length-label");

  // console.log(treatedLabel);

  if (treatedLabel.length === 0) {
    label.innerHTML = "??mm";
    label.style.opacity = 0.2;
  } else {
    label.innerHTML = (treatedLabel.length < 7 ? 7 : treatedLabel.length) * 4 + "mm";
    label.style.opacity = 1;
  }
}

async function labelCommand() {
  // sends the label to the device

  let fieldValue = document.getElementById("text-input").value;
  if (useRegex(fieldValue)) {
    // console.log('printing: "' + treatedLabel.toLowerCase() + '"');
    document.getElementById("text-input").blur();
    setUiBusy(true);
    let response = await postJson("api/task", {parameter: "tag", value: treatedLabel.toLowerCase()});
    if (!response.ok) {
      console.error("Unable to feed");
      console.error((await response.json())["error"])
    }
  }
}

function drawHelper() {
  // draws visual helper with label length taking options into account

  let field = document.getElementById("text-input");
  let fieldValue = field.value;
  let space = "X";
  let multiplier = 0;
  let mode = document.getElementById("mode-dropdown").value;
  switch (mode) {
    case "margin":
      minSize = 12;
      field.maxLength = 18;
      if (field.value.length > 18) {
        field.value = fieldValue.slice(0, 18);
      }
      multiplier = 1;
      break;
    case "tight":
      minSize = 10;
      multiplier = 0;
      field.maxLength = 20;
      break;
    case "full":
      minSize = 20;
      multiplier = Math.floor((20 - field.value.length) / 2);
      field.maxLength = 20;
      break;
  }

  document.getElementById("clear-button").disabled = fieldValue === "";
  document.getElementById("submit-button").disabled = fieldValue === "";
  document.getElementById("reel-button").disabled = !(fieldValue === "");
  document.getElementById("feed-button").disabled = !(fieldValue === "");
  document.getElementById("cut-button").disabled = !(fieldValue === "");
  document.getElementById("setup-button").disabled = !(fieldValue === "");

  if (fieldValue != "" && !clear) {
    if (multiplier + field.value.length + multiplier < 7) {
      multiplier = Math.ceil((7 - field.value.length) / 2);
      // console.log("minimum length " + multiplier);
    }
    document.getElementById("size-helper-content").innerHTML =
      space.repeat(multiplier) + "x".repeat(field.value.length) + space.repeat(multiplier);

    treatedLabel = " ".repeat(multiplier) + field.value + " ".repeat(multiplier);
    // console.log('"' + treatedLabel + '"');
  } else {
    clear = false;
    treatedLabel = "";
    document.getElementById("size-helper-content").innerHTML =
      space.repeat(multiplier) + (mode != "full" ? "WRITE HERE" : "") + space.repeat(multiplier);
  }
}

function validateField() {
  // instantly validates label field by blocking buttons and giving visual feedback

  let field = document.getElementById("text-input");
  let fieldValue = field.value;

  let valid;

  drawHelper();

  if (!useRegex(fieldValue) && fieldValue != "") {
    valid = false;
    document.getElementById("hint").style.color = "red";
    document.getElementById("text-input").style.color = "red";
    document.getElementById("submit-button").disabled = true;
    document.getElementById("submit-button").value = " invalid entry ";
    document.getElementById("submit-button").style.color = "red";
  } else {
    valid = true;
    document.getElementById("hint").style.color = "#e7dac960";
    document.getElementById("text-input").style.color = "#e7dac9ff";
    document.getElementById("submit-button").value = fieldValue != "" ? " Print label! " : " ... ";
    document.getElementById("submit-button").style.color = "#e7dac9ff";
  }
}

function formKeyHandler(e) {
  // sends the form by using the enter key

  let keynum;
  if (window.event && e != null) {
    keynum = e.keyCode;
  } else if (e.which && e != null) {
    keynum = e.which;
  }

  validateField();

  if (keynum === 13 && valid) {
    document.getElementById("submit-button").click();
  }
}

function clearField() {
  // clears the label field and restore default button and form states

  clear = true;
  const textField = document.getElementById("text-input");

  document.getElementById("clear-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("reel-button").disabled = false;
  document.getElementById("feed-button").disabled = false;
  document.getElementById("cut-button").disabled = false;
  document.getElementById("hint").style.color = "#777777";
  document.getElementById("text-input").style.color = "#ffffff";
  document.getElementById("submit-button").value = " ... ";

  textField.value = "";
  treatedLabel = "";

  drawHelper();
  calculateLength();

  textField.focus();
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

  const textarea = document.getElementById("text-input");
  textarea.focus();

  let insertStartPoint;
  let insertEndPoint;
  let value = textarea.value;

  if (textarea.selectionStart == textarea.selectionEnd) {
    insertStartPoint = textarea.selectionStart;
    insertEndPoint = insertStartPoint;
  } else {
    insertStartPoint = textarea.selectionStart;
    insertEndPoint = textarea.selectionEnd;
  }

  // text before cursor/highlighted text + special character + text after cursor/highlighted text
  value = value.slice(0, insertStartPoint) + specialChar + value.slice(insertEndPoint);
  textarea.value = value;

  textarea.setSelectionRange(insertStartPoint + 1, insertStartPoint + 1);

  validateField();

  drawHelper();
  calculateLength();

  textarea.focus();
}

function useRegex(input) {
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
    let response = await postJson("api/task", {parameter: "reel", value: ""});
    if (!response.ok) {
      console.error("Unable to reel");
      console.error((await response.json())["error"])
    }
  }
}

async function feedCommand() {
  // sends feed command to the device
  setUiBusy(true);
  document.getElementById("submit-button").value = " feeding... ";
  let response = await postJson("api/task", {parameter: "feed", value: ""});
  if (!response.ok) {
    console.error("Unable to feed");
    console.error((await response.json())["error"])
  }
}

async function cutCommand() {
  // sends cut command to the device
  setUiBusy(true);
  document.getElementById("submit-button").value = " cutting... ";
  let response = await postJson("api/task", {parameter: "cut", value: ""});
  if (!response.ok) {
    console.error("Unable to cut");
    console.error((await response.json())["error"])
  }
}

async function testCommand(testFull) {
  // sends test command to the device
  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;
  let data;
  if (testFull) {
    data = {
      parameter: 'testfull',
      value: align + "," + force
    };
  } else {
    data = {
      parameter: 'testalign',
      value: align + "," + 1
    };
  }
  setUiBusy(true);
  let response = await postJson("api/task", data);
  if (!response.ok) {
    console.error("Unable to perform test");
    console.error((await response.json())["error"])
  }
}

function settingsCommand() {
  // sends settings save command to the device, and triggers self restart in 15 seconds

  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;

  // console.log("settings / align (" + align + ") force (" + force + ")");

  if (confirm("Confirm saving align [" + align + "] and force [" + force + "] settings?")) {
    // console.log("settings sent");
    var xhr = new XMLHttpRequest();
    // xhr.open("GET", "/&?" + "settings", true);
    xhr.open("GET", "/&?" + "save=" + align + "," + force, true);
    xhr.send();
    command = "test";
    busy = true;

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
    signal: controller.signal  
  });
  clearTimeout(id);
  return response;
}


// Helper method to post a json request, supports timeouts.
async function postJson(url, data, options = {}) {
  options.headers = {
    'Content-Type': 'application/json',
    'Accept': 'application/json'
  };
  options.body = JSON.stringify(data);
  options.method = 'POST';
  return await fetchWithTimeout(url, options);
}


async function getStatus() {
  try {
    let request = await fetchWithTimeout("api/status", {timeout: 5000});
    handleData(await request.json());
  } catch (error) {
    // TODO: Add some UI treatment for when there are communication errors, eg 
    // a "Reconnecting..." toast message or something.
    console.error("Problem ")
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
    const textField = document.getElementById("text-input");
    Array.from(document.querySelectorAll("input")).forEach((element) => {
      element.disabled = false;
    });
    document.getElementById("mode-dropdown").disabled = false;
    document.getElementById("submit-button").disabled = textField.value == "";
    document.getElementById("clear-button").disabled = textField.value == "";
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

  switch (data_json.command) {
    case "tag":
      document.getElementById("submit-button").value = " printing " + percentage + "% ";
      let body = document.getElementsByTagName("body")[0];
      body.dataset.printing = "true";
      document.getElementById("progress-bar").style.width = percentage.toString() + "%";
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
