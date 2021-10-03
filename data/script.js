function toggleVisibility(x, y) {
  if (x.style.visibility === "hidden") {
    x.style.visibility = "visible";
    y.innerHTML = "- [ EXTRAS ] -";
  } else if (x.style.visibility === "visible") {
    x.style.visibility = "hidden";
    y.innerHTML = " + [ EXTRAS ] + ";
  }
}

function fadeSplash(){
  
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
