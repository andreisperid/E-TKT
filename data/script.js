document.getElementById("text-input").focus();
let busy = false;
let command = "";
let treatedLabel = "";
let clear = false;
let minSize = 7;

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

  // console.log(treatedLabel);

  if (treatedLabel.length === 0) {
    label.innerHTML = "??mm";
  } else {
    label.innerHTML =
      (treatedLabel.length < 7 ? 7 : treatedLabel.length) * 4 + "mm";
  }
}

function tagCommand() {
  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {
    // console.log('printing: "' + treatedLabel.toLowerCase() + '"');
    document.getElementById("text-input").blur();
    document.getElementById("text-input").disabled = true;
    document.getElementById("clear-button").disabled = true;
    document.getElementById("submit-button").disabled = true;
    document.getElementById("reel-button").disabled = true;
    document.getElementById("feed-button").disabled = true;
    document.getElementById("cut-button").disabled = true;
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?" + "tag=" + treatedLabel.toLowerCase(), true);
    xhr.send();
    command = "tag";
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

  if (fieldValue != "" && !clear) {
    if (multiplier + field.value.length + multiplier < 7) {
      multiplier = Math.ceil((7 - field.value.length) / 2);
      // console.log("minimum length " + multiplier);
    }
    document.getElementById("size-helper").innerHTML =
      space.repeat(multiplier) +
      "x".repeat(field.value.length) +
      space.repeat(multiplier);

    treatedLabel =
      "_".repeat(multiplier) + field.value + "_".repeat(multiplier);
    // console.log('"' + treatedLabel + '"');
  } else {
    clear = false;
    treatedLabel = "";
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
    document.getElementById("submit-button").value = " invalid entry ";
    document.getElementById("submit-button").style.color = "red";
  } else {
    valid = true;
    document.getElementById("hint").style.color = "#777777";
    document.getElementById("text-input").style.color = "#ffffff";
    document.getElementById("submit-button").value =
      fieldValue != "" ? " Print label! " : " ... ";
    document.getElementById("submit-button").style.color = "white";
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

function useRegex(input) {
  let regex = /^[a-zA-Z0-9 .\-]+$/i;
  return regex.test(input);
}

function reelCommand() {
  if (confirm("Confirm loading a new reel? \n\n Please make sure the tape is touching the cog.")) {
    document.getElementById("reel-button").disabled = true;
    document.getElementById("feed-button").disabled = true;
    document.getElementById("cut-button").disabled = true;
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
  document.getElementById("reel-button").disabled = true;
  document.getElementById("feed-button").disabled = true;
  document.getElementById("cut-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("submit-button").value = " feeding... ";
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "feed", true);
  xhr.send();
  command = "feed";
  busy = true;
}

function cutCommand() {
  document.getElementById("reel-button").disabled = true;
  document.getElementById("feed-button").disabled = true;
  document.getElementById("cut-button").disabled = true;
  document.getElementById("submit-button").disabled = true;
  document.getElementById("submit-button").value = " cutting... ";

  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?" + "cut", true);
  xhr.send();
  command = "cut";
  busy = true;
}

setInterval(function () {
  getData();
}, 100);

function getData() {
  textField = document.getElementById("text-input");
  // console.log("getting data");
  // console.log(textField.value == "");

  if (busy) {
    // console.log("busy")

    var xhttp = new XMLHttpRequest();
    xhttp.onreadystatechange = function () {
      if (this.readyState == 4 && this.status == 200) {
        // console.log("response: " + this.responseText);

        switch (command) {
          case "tag":
            document.getElementById("submit-button").value =
              " Printing " + this.responseText + "% ";
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
          document.getElementById("progress-bar").style.width = 0;
          document.getElementById("submit-button").value = " Print label! ";
          document.getElementById("text-input").disabled = false;
          document.getElementById("clear-button").disabled =
            textField.value == "";
          document.getElementById("submit-button").disabled =
            textField.value == "";
          document.getElementById("reel-button").disabled = false;
          document.getElementById("feed-button").disabled = false;
          document.getElementById("cut-button").disabled = false;
          busy = false;
        }
      }
    };
    xhttp.open("GET", "status", true);
    xhttp.send();
  }
}
