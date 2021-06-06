function toggleVisibility(x, y) {
  if (x.style.visibility === "hidden") {
    x.style.visibility = "visible";
    y.innerHTML = "- EXTRAS -";
  } else if (x.style.visibility === "visible") {
    x.style.visibility = "hidden";
    y.innerHTML = " + [ EXTRAS ] + ";
  }
}

function tagCommand() {
  let fieldValue = document.getElementById("text-input").value;

  if (useRegex(fieldValue)) {    
    window.open("&?tag=" + fieldValue, "_self");

  } else {
    document.getElementById("hint").style.color = "red";    
    document.getElementById("hint").style.textDecoration = "underline";
    window.alert("allowed: A-Z 0-9 space . - * (20 max)")
  }
}

function clearWarnings(){
    document.getElementById("hint").style.color = "white";    
    document.getElementById("hint").style.textDecoration = "none";
}

function useRegex(input) {
    let regex = /^[a-zA-Z0-9 .*]+$/i;
    return regex.test(input);
}

function rwCommand() {
  window.open("&?rw", "_self");
}

function fwCommand() {
  window.open("&?fw", "_self");
}

function cutCommand() {
  window.open("&?cut", "_self");
}
