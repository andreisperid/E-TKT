function toggleVisibility(x, y) {
    if (x.style.visibility === "hidden") {
        x.style.visibility = "visible";
        y.innerHTML  = "- EXTRAS -";
    } else if (x.style.visibility === "visible") {
        x.style.visibility = "hidden";
        y.innerHTML  = " + [ EXTRAS ] + ";
    }
}

function tagCommand(){
    window.open("&?tag=" + document.getElementById("text-input").value,"_self")
}

function rwCommand(){
    window.open("&?rw","_self")
}

function fwCommand(){
    window.open("&?fw","_self")
}

function cutCommand(){
    window.open("&?cut","_self")
}