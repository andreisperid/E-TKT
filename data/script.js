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

window.load = startupRoutine();

function startupRoutine() {
  document.getElementById("text-input").focus();
  retrieveSettings();
}

function retrieveSettings() {
  // retrieve settings from the device

  // console.log("trying to retrieve settings...");

  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      // console.log("settings read: '" + this.responseText + "'");

      align = this.responseText[0];
      force = this.responseText[1];
      alignTemp = align;
      forceTemp = force;

      document.getElementById("align-field").value = align;
      document.getElementById("force-field").value = force;
    }
  };
  xhttp.open("GET", "settings", true);
  xhttp.send();
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

function labelCommand() {
  // sends the label to the device

  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {
    // console.log('printing: "' + treatedLabel.toLowerCase() + '"');
    document.getElementById("text-input").blur();

    document.getElementById("size-helper").style.borderColor = "rgba(128, 128, 128, 0)";
    document.getElementById("size-helper").style.borderStyle = "solid";
    document.getElementById("size-helper").style.backgroundColor = "rgba(32, 32, 32, 1)";
    document.getElementById("size-helper").style.mixBlendMode = "difference";
    document.getElementById("progress-bar").style.outline = "solid 2px white";

    document.getElementById("mode-dropdown").disabled = true;
    let emojiDivs = document.getElementById("emoji-buttons").children;
    Array.from(emojiDivs).forEach((element) => {
      element.children[0].disabled = true;
    });
    document.getElementById("text-input").disabled = true;
    document.getElementById("clear-button").disabled = true;
    document.getElementById("submit-button").disabled = true;
    document.getElementById("reel-button").disabled = true;
    document.getElementById("feed-button").disabled = true;
    document.getElementById("cut-button").disabled = true;
    document.getElementById("setup-button").disabled = true;

    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "tag=" + treatedLabel.toLowerCase(), true);
    xhr.send();
    command = "tag";
    busy = true;
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

    treatedLabel = "_".repeat(multiplier) + field.value + "_".repeat(multiplier);
    // console.log('"' + treatedLabel + '"');

    // conversion table to prevent multichar error
    treatedLabel = treatedLabel.replace(/♡/g, "<");
    treatedLabel = treatedLabel.replace(/☆/g, ">");
    treatedLabel = treatedLabel.replace(/♪/g, "~");
    treatedLabel = treatedLabel.replace(/€/g, "|");
    // console.log(treatedLabel);
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

function reelCommand() {
  // sends reel command to the device

  if (
    confirm(
      "Confirm loading a new reel?\n\nPlease make sure the tape is touching the cog.\n\nImportant: unsaved align and force settings will be lost."
    )
  ) {
    toggleSettings(false);
    document.getElementById("mode-dropdown").disabled = true;
    let emojiDivs = document.getElementById("emoji-buttons").children;
    Array.from(emojiDivs).forEach((element) => {
      element.children[0].disabled = true;
    });
    document.getElementById("text-input").disabled = true;
    document.getElementById("clear-button").disabled = true;
    document.getElementById("reel-button").disabled = true;
    document.getElementById("feed-button").disabled = true;
    document.getElementById("cut-button").disabled = true;
    document.getElementById("setup-button").disabled = true;
  
    document.getElementById("submit-button").disabled = true;
    document.getElementById("submit-button").value = "  reeling... ";
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "reel", true);
    xhr.send();
    command = "reel";
    busy = true;
  }
}

function feedCommand() {
  // sends feed command to the device

  document.getElementById("mode-dropdown").disabled = true;
  let emojiDivs = document.getElementById("emoji-buttons").children;
  Array.from(emojiDivs).forEach((element) => {
    element.children[0].disabled = true;
  });
  document.getElementById("text-input").disabled = true;
  document.getElementById("clear-button").disabled = true;
  document.getElementById("reel-button").disabled = true;
  document.getElementById("feed-button").disabled = true;
  document.getElementById("cut-button").disabled = true;
  document.getElementById("setup-button").disabled = true;

  document.getElementById("submit-button").disabled = true;
  document.getElementById("submit-button").value = " feeding... ";
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "feed", true);
  xhr.send();
  command = "feed";
  busy = true;
}

function cutCommand() {
  // sends cut command to the device

  document.getElementById("mode-dropdown").disabled = true;
  let emojiDivs = document.getElementById("emoji-buttons").children;
  Array.from(emojiDivs).forEach((element) => {
    element.children[0].disabled = true;
  });
  document.getElementById("text-input").disabled = true;
  document.getElementById("clear-button").disabled = true;
  document.getElementById("reel-button").disabled = true;
  document.getElementById("feed-button").disabled = true;
  document.getElementById("cut-button").disabled = true;
  document.getElementById("setup-button").disabled = true;

  document.getElementById("submit-button").disabled = true;
  document.getElementById("submit-button").value = " cutting... ";

  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "cut", true);
  xhr.send();
  command = "cut";
  busy = true;
}

function testCommand(testFull) {
  // sends test command to the device

  document.getElementById("add-align-button").disabled = true;
  document.getElementById("remove-align-button").disabled = true;
  document.getElementById("add-force-button").disabled = true;
  document.getElementById("remove-force-button").disabled = true;

  document.getElementById("test-full-button").disabled = true;
  document.getElementById("test-align-button").disabled = true;

  document.getElementById("reel-button").disabled = true;
  document.getElementById("align-field").disabled = true;
  document.getElementById("force-field").disabled = true;
  document.getElementById("cancel-button").disabled = true;
  document.getElementById("save-button").disabled = true;
  align = document.getElementById("align-field").value;
  force = document.getElementById("force-field").value;

  if (testFull) {
    // console.log("test full / align: " + align + ", force: " + force);
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "testfull=" + align + "," + force, true);
    xhr.send();
    command = "testfull";
  } else {
    // console.log("test align : " + align);
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "testalign=" + align + "," + 1, true);
    xhr.send();
    command = "testalign";
  }

  busy = true;
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

setInterval(function () {
  getData();
}, 100); // lower rate?

function getData() {
  // gets progress data from the device and disable buttons while busy

  const textField = document.getElementById("text-input");
  // console.log("getting data");
  // console.log(textField.value == "");

  if (busy) {
    // console.log("busy")

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        // console.log("response: " + this.responseText);

        let percentage = parseInt(this.responseText)
        if (percentage > 0){
          percentage -= 1 // avoid 100% progress while still finishing
        }

        switch (command) {
          case "tag":
            document.getElementById("submit-button").value = " printing " + percentage + "% ";
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

        document.getElementById("progress-bar").style.width =
          (this.responseText === "" ? " 0" : this.responseText) + "%";

        if (this.responseText === "finished") {
          // main
          document.getElementById("progress-bar").style.width = 0;
          document.getElementById("submit-button").value = " Print label! ";
          document.getElementById("text-input").disabled = false;
          document.getElementById("clear-button").disabled = textField.value == "";
          document.getElementById("submit-button").disabled = textField.value == "";
          document.getElementById("reel-button").disabled = false;
          document.getElementById("feed-button").disabled = false;
          document.getElementById("cut-button").disabled = false;
          document.getElementById("setup-button").disabled = false;

          document.getElementById("mode-dropdown").disabled = false;

          let emojiDivs = document.getElementById("emoji-buttons").children;
          Array.from(emojiDivs).forEach((element) => {
            element.children[0].disabled = false;
          });

          document.getElementById("size-helper").style.borderLeftColor = "rgba(0, 102, 255, 0.7)";
          document.getElementById("size-helper").style.borderRightColor = "rgba(0, 102, 255, 0.7)";
          document.getElementById("size-helper").style.borderTopColor = "rgba(255, 166, 0, 0.4) ";
          document.getElementById("size-helper").style.borderBottomColor = "rgba(255, 166, 0, 0.4)";
          document.getElementById("size-helper").style.borderStyle = "solid dashed solid dashed";

          document.getElementById("size-helper").style.backgroundColor = "rgba(0, 0, 0, 0)";
          document.getElementById("size-helper").style.mixBlendMode = "normal";

          document.getElementById("progress-bar").style.outline = "none";

          // settings
          document.getElementById("add-align-button").disabled = false;
          document.getElementById("remove-align-button").disabled = false;
          document.getElementById("add-force-button").disabled = false;
          document.getElementById("remove-force-button").disabled = false;
          document.getElementById("test-align-button").disabled = false;
          document.getElementById("test-full-button").disabled = false;
          document.getElementById("cancel-button").disabled = false;
          document.getElementById("save-button").disabled = false;

          busy = false;
        }
      }
    };
    xhttp.open("GET", "status", true);
    xhttp.send();
  }
}
