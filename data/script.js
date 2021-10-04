function toggleVisibility(x, y, openedLabel, closedLabel) {
  if (x.style.visibility === "hidden") {
    x.style.visibility = "visible"
    x.style.height = "unset"
    y.innerHTML = openedLabel;
  } else if (x.style.visibility === "visible") {
    x.style.visibility = "hidden" 
    x.style.height = "0px"
    y.innerHTML = closedLabel;
  }
}

function calculateLength(){
  let label = document.getElementById("length-label");
  let input = document.getElementById("text-input").value;

  label.innerHTML =  ((input.length < 7 ? 7 : input.length) * 4) + "mm"
}


function tagCommand() {
  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?"+"tag=" + fieldValue.toLowerCase(), true);
    xhr.send();
  } else {
    document.getElementById("hint").style.color = "red";    
    document.getElementById("hint").style.textDecoration = "underline";
    window.alert("allowed: A-Z 0-9 space . - (20 max)")
  }
}

function clearWarning(){
  document.getElementById("hint").style.color = "#777777";    
  document.getElementById("hint").style.textDecoration = "none";
}

function clearField(){
    document.getElementById("text-input").value = "";    
}

function useRegex(input) {
    let regex = /^[a-zA-Z0-9 .]+$/i;
    return regex.test(input);
}

function rwCommand() {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", "/&?"+"rw", true);
  xhr.send();
}

function fwCommand() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?"+"fw", true);
    xhr.send();
}

function cutCommand() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/&?"+"cut", true);
    xhr.send();
}
