document.getElementById("text-input").focus();

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
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "tag=" + fieldValue.toLowerCase(), true);
    xhr.send();
    fieldValue.blur();
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

  let fieldValue = document.getElementById("text-input").value;
  let valid;

  document.getElementById("clear-button").disabled = fieldValue === "";
  document.getElementById("submit-button").disabled = fieldValue === "";

  if (!useRegex(fieldValue) && fieldValue != "") {
    valid = false;
    document.getElementById("hint").style.color = "red";
    document.getElementById("text-input").style.color = "red";
  } else {
    valid = true;
    document.getElementById("hint").style.color = "#777777";
    document.getElementById("text-input").style.color = "#ffffff";
  }

  if (keynum === 13 && valid) {
    // document.getElementById("text-input").value = "eita"
    // Cancel the default action, if needed
    // preventDefault();
    // Trigger the button element with a click
    document.getElementById("submit-button").click();
    
    document.getElementById("text-input").blur();
    // tagCommand();
  }
}

function clearField() {
  textField = document.getElementById("text-input");
  
  document.getElementById("clear-button").disabled = true;
  document.getElementById("submit-button").disabled = true;

  textField.value = "";
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
