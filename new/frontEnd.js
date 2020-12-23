window.addEventListener('load', () => {
    const canvas = document.querySelector('canvas');
    const ctx = canvas.getContext("2d");

    canvas.height = window.innerHeight * .9;
    canvas.width = window.innerWidth;

    let painting = false;

    function startPosition(e) {
        paintign = true;
        draw(e);
    }

    function finishedPosition() {
        painting = false;
        ctx.beginPath()
    }

    function draw(e) {
        if (!painting) {
            return
        }
        ctx.lineWidth = 10;
        ctx.lineCap = "round";
        ctx.strokeStyle

        ctx.lineTo(e.clientX, e.clientY);
        ctx.stroke();
        ctx.beginPath();
        ctx, moveTo(e.clientX, e.clientY);
    }

    canvas.addEventListener("mousedown", startPosition);
    canvas.addEventListener("mouseup", finishedPosition);
    canvas.addEventListener("mouseover", draw)
})