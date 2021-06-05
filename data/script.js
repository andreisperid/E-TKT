function toggleVisibility(x, y) {
    if (x.style.visibility === "hidden") {
        x.style.visibility = "visible";
        y.innerHTML  = "- EXTRAS -";
    } else if (x.style.visibility === "visible") {
        x.style.visibility = "hidden";
        y.innerHTML  = " + [ EXTRAS ] + ";
    }
}