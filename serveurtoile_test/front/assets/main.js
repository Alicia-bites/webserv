function randomHexColor() {
    return '#' + Math.floor(Math.random() * 16777215).toString(16);
}

function generateurDollar() {
    const val = $(this).val();
    if (val > 100)
    {
        $("#porte-monnaie").html("<h1 style='color: red;'>trop d'argent!!!!!!!!!!</h1>");
        return;
    }
    $("#porte-monnaie").html('');
    for (let i = 0; i < val; i++) {
        $("#porte-monnaie").append('<div class="anim-spin"><span><img src="assets/dollarspindownd.gif">argent</span></div>');
    }
}

$(function() {
    setInterval(() => {
        $("#title").css("color", randomHexColor());
    }, 250);

    $("#dollar-count").change(generateurDollar).keyup(generateurDollar);
});
