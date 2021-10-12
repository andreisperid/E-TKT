document.getElementById("text-input").focus();
let busy = false;
let treatedLabel = "";
let clear = false;

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
  label.innerHTML =
    (treatedLabel.length < 7 ? 7 : treatedLabel.length) * 4 + "mm";
}

function tagCommand() {
  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {
    console.log('printing: "' + treatedLabel.toLowerCase() + '"');
    document.getElementById("text-input").blur();
    document.getElementById("text-input").disabled = true;
    document.getElementById("clear-button").disabled = true;
    document.getElementById("submit-button").disabled = true;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "tag=" + treatedLabel.toLowerCase(), true);
    xhr.send();
    busy = true;
  }
}

function drawHelper() {
  let field = document.getElementById("text-input");
  let fieldValue = field.value;
  let space = "X";
  let multiplier = 0;
  let mode = document.getElementById("mode-dropdown").value;
  switch (mode) {
    case "margin":
      field.maxLength = 18;
      if (field.value.length > 18) {
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
      break;
  }

  document.getElementById("clear-button").disabled = fieldValue === "";
  document.getElementById("submit-button").disabled = fieldValue === "";

  if (fieldValue != "" && !clear) {
    if (multiplier + field.value.length + multiplier < 7) {
      multiplier = Math.ceil((7 - field.value.length) / 2);
      console.log("minimum length " + multiplier);
    }
    document.getElementById("size-helper").innerHTML =
      space.repeat(multiplier) +
      "x".repeat(field.value.length) +
      space.repeat(multiplier);

    treatedLabel =
      "_".repeat(multiplier) + field.value + "_".repeat(multiplier);
    console.log('"' + treatedLabel + '"');
  } else {
    clear = false;
    document.getElementById("size-helper").innerHTML =
      space.repeat(multiplier) +
      (mode != "full" ? "WRITE HERE" : "") +
      space.repeat(multiplier);
  }
}

function formKeyHandler(e) {
  let field = document.getElementById("text-input");
  let fieldValue = field.value;


  let keynum;
  if (window.event && e != null) {
    keynum = e.keyCode;
  } else if (e.which && e != null) {
    keynum = e.which;
  }

  let valid;

  drawHelper();

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
  clear = true;
  textField = document.getElementById("text-input");

  document.getElementById("clear-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("hint").style.color = "#777777";
  document.getElementById("text-input").style.color = "#ffffff";

  textField.value = "";
  treatedLabel = "";

  drawHelper();
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
  if (busy) {
    getData();
  }
}, 500);

function getData() {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function () {
    if (this.readyState == 4 && this.status == 200) {
      console.log(this.responseText);
      document.getElementById("submit-button").value =
        " Printing " + this.responseText + "% ";
      document.getElementById("progress-bar").style.width =
        (this.responseText === "" ? " 0" : this.responseText) + "%";

      if (this.responseText === "finished" || this.responseText === "" ) {
        busy = false;
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
