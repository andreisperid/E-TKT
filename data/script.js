document.getElementById("text-input").focus();
let busy = false;
let previousInputLength;

function toggleVisibility(x, y, openedLabel, closedLabel) {
  if (x.style.visibility === "hidden") {
    x.style.visibility = "visible";
    x.style.height = "unset";
    y.innerHTML = openedLabel;
  } else if (x.style.visibility === "visible") {
    x.style.visibility = "hidden";
    x.style.height = "0px";
    y.innerHTML = closedLabel;
  }
}

function calculateLength() {
  let label = document.getElementById("length-label");
  let input = document.getElementById("text-input").value;

  label.innerHTML = (input.length < 7 ? 7 : input.length) * 4 + "mm";
}

function tagCommand() {
  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {
    console.log('printing: "' + fieldValue.toLowerCase() + '"');

    document.getElementById("text-input").blur();
    document.getElementById("text-input").disabled = true;
    document.getElementById("clear-button").disabled = true;
    document.getElementById("submit-button").disabled = true;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "tag=" + fieldValue.toLowerCase(), true);
    xhr.send();
    busy = true;
  }
}

function formKeyHandler(e) {
  let keynum;
  if (window.event) {
    // IE
    keynum = e.keyCode;
  } else if (e.which) {
    // Netscape/Firefox/Opera
    keynum = e.which;
  }
  let field = document.getElementById("text-input");
  let fieldValue = field.value;
  let valid;

  let space = "X";
  let multiplier = 0;
  let mode = document.getElementById("mode-dropdown").value;
  switch (mode) {
    case "margin":
      field.maxLength = 18;
      if (field.value.length > 18) {
        console.log("slicing string");
        field.value = fieldValue.slice(0, 18);
      }
      multiplier = 1;
      break;
    case "tight":
      multiplier = 0;
      field.maxLength = 20;
      break;
    case "full":
      multiplier = Math.floor((20 - field.value.length) / 2);
      field.maxLength = 20;
      console.log(previousInputLength + " / " +  field.value.length)
      if (
        previousInputLength !== field.value.length ||
        previousInputLength == 0
      ) {
        let realvalue = field.value.trim();
        console.log('trimmed: "' + realvalue + '"');
        field.valid = realvalue;

        if (realvalue.length % 2 != 0) {
          field.value += " ";
          field.selectionStart = field.selectionEnd = field.value.length - 1;
        }
      }
      previousInputLength = field.value.length;

      break;
  }

  document.getElementById("clear-button").disabled = fieldValue === "";
  document.getElementById("submit-button").disabled = fieldValue === "";

  if (fieldValue != "") {
    document.getElementById("size-helper").innerHTML =
      space.repeat(multiplier) +
      "o".repeat(field.value.length) +
      space.repeat(multiplier) +
      (mode === "full" ? space.repeat((20 - fieldValue.length) % 2) : "");
  } else {
    document.getElementById("size-helper").innerHTML =
      space.repeat(multiplier) +
      (mode != "full" ? "WRITE HERE" : "") +
      space.repeat(multiplier);
  }

  if (!useRegex(fieldValue) && fieldValue != "") {
    valid = false;
    document.getElementById("hint").style.color = "red";
    document.getElementById("text-input").style.color = "red";
    document.getElementById("submit-button").disabled = true;
  } else {
    valid = true;
    document.getElementById("hint").style.color = "#777777";
    document.getElementById("text-input").style.color = "#ffffff";
  }

  if (keynum === 13 && valid) {
    document.getElementById("submit-button").click();
  }
}

function clearField() {
  textField = document.getElementById("text-input");

  document.getElementById("clear-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("hint").style.color = "#777777";
  document.getElementById("text-input").style.color = "#ffffff";

  textField.value = "";
  calculateLength();

  textField.focus();
}

function useRegex(input) {
  let regex = /^[a-zA-Z0-9 .]+$/i;
  return regex.test(input);
}

function rwCommand() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "rw", true);
  xhr.send();
}

function fwCommand() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "fw", true);
  xhr.send();
}

function cutCommand() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "cut", true);
  xhr.send();
}

setInterval(function () {
  // Call a function repetatively with 2 Second interval
  if (busy) {
    getData();
  }
  // console.log("interval");
}, 500); //2000mSeconds update rate

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      // document.getElementById("ADCValue").innerHTML = this.responseText;
      console.log(this.responseText);
      document.getElementById("submit-button").value =
        " Printing " + this.responseText + "% ";
      document.getElementById("progress-bar").style.width =
        this.responseText + "%";

      if (this.responseText === "finished") {
        busy = false;
        // document.getElementById("text-input").blur();
        document.getElementById("progress-bar").style.width = 0;
        document.getElementById("submit-button").value = " Print label! ";
        document.getElementById("text-input").disabled = false;
        document.getElementById("clear-button").disabled = false;
        document.getElementById("submit-button").disabled = false;
      }
    }
  };
  xhttp.open("GET", "status", true);
  xhttp.send();
}
