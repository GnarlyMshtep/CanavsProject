console.log('client js running :)');

const addActionButt = document.querySelector(".form-add-action-butt");
const launchActionsButt = document.querySelector(".run-actions-butt");
const canvas = document.querySelector(".canvas-display");
const distInp = document.querySelector(".form-cm-input");
const drawBoolInp = document.querySelector(".radio-bttn"); //gets only the first radio  button
const selDirInput = document.querySelector(".form-select-direc");
const actDispBox = document.querySelector(".action-display-container");
const ctx = document.querySelector(".canvas-display").getContext("2d");
const canActHei = 7;
const canActWid = 8;
let invalidPosPresent = false;


let canPos = {
    'posX': canvas.offsetWidth / 2,
    'posY': canvas.offsetHeight / 2
};

let actions = [];


addActionButt.addEventListener('click', addAction, false);
launchActionsButt.addEventListener('click', launchActions, false);

//able deletion of action

function addAction() {
    /*do stuff when action is added 
        1.create action object
        2.display it to the action display
        3.add action to array of all actions
    */
    let action = getFormData() //return an object {distance float, drawBool, direction string}
    console.log(action);
    actions.push(action);
    refreshHtmlActions();
    refreshCanvasDisplay();
    // action.moveOnCanvas()
}



function launchActions() {
    /*do stuff when actions are added
        1.convert actions to HTTP post format
        2.send to the server

        send the first action
        await confirmation that its done
        send the second
    */
}

function getFormData() {
    return new Action(
        (distInp.value.length > 0) ? parseFloat(distInp.value) : 1,
        (drawBoolInp.checked),
        selDirInput.value
    );
}

function initCanvas() {
    canPos.posX = canvas.offsetWidth / 2;
    canPos.posY = canvas.offsetHeight / 2;

    ctx.clearRect(0, 0, canvas.width, canvas.height); //clear html5 canvas

    ctx.fillStyle = "green";
    ctx.lineWidth = 5;

    ctx.beginPath();    
    ctx.moveTo(canPos.posX, canPos.posY);
}

function refreshCanvasDisplay() {
    initCanvas(); //init canvas to middle and no actions

    for (let i = 0; i < actions.length; i++) { //run thru all actions
        actions[i].moveCtxPos(); //move the canvas position but dont make the change on the canvas quite yes

        if (canPos.posX < 0 || canPos.posY < 0) { //validation for too small
            invalidPosPresent = true;
            console.error("action number " + i + "has existed the canvas boundery (canPos.posX<0||canPos.posY<0)");
        }
        if (canPos.posX > canvas.offsetWidth || canPos.posY > canvas.offsetHeight) { //validation for too big
            invalidPosPresent = true;
            console.error("action number " + i + "has existed the canvas boundery (canPos.posX > canvas.offsetWidth || canPos.posY > canvas.offsetHeight)");
        } else if (actions[i].draw) { //draw if draw
            ctx.lineTo(canPos.posX, canPos.posY);
            console.log("invoked draw");
        } else if (!(actions[i].draw)) { //move if no draw and error
            ctx.moveTo(canPos.posX, canPos.posY);
        }
        invalidPosPresent = false;
        ctx.stroke()
    }

}

function refreshHtmlActions() {
    actDispBox.innerHTML = ""; //delete everything in the box

    //every time an action is added or popped from
    // actions this should be invoked to redraw the actions\

    for (let i = 0; i < actions.length; i++) {
        actDispBox.innerHTML += (actions[i].toHtmlString(i + 1));
    }
}

class Action {

    constructor(length0, draw0, direction0) {
        //len in cm draw bool draw or just move marker
        //direction up down left or right
        this.len = length0;
        this.draw = draw0;
        this.direc = direction0;
        this.drawStr = this.draw ? "DRAW" : "NO DRAW";
    }

    toPost() {

    }

    moveCtxPos() {
        let pxLen = (3 / 100) * window.innerWidth * this.len;
        console.log("pxLen is: " + pxLen);
        switch (this.direc) {
            case "up":
                canPos.posY -= pxLen;
                break;
            case "right":
                canPos.posX -= pxLen;
                break;
            case "down":
                canPos.posY += pxLen;
                break;
            case "left":
                canPos.posX -= pxLen;
                break;
            default:
                console.log("Something isn't working, an action with no direc entered the array.");
        }
    }

    toHtmlString(num) {
        return `<div class="action-box">
        <!--later wanna add a way to rearraneg-->
        <span class="action-num">` + String(num) + `</span>
        <span class="delete-action-x">X</span>

        <div class="action-desc">
            <p class="action-draw">` + this.drawStr + `</p>
            <p class="action-len">` + String(this.len) + ` CM</p>
            <p class="action-direc">` + (this.direc).toUpperCase() + `</p>
        </div>
    </div> 
    `
    }

}

/* 
<div class="action-box">
    <!--later wanna add a way to rearraneg-->
    <span class="action-num">1</span>
    <span class="delete-action-x">X</span>

    <div class="action-desc">
        <p class="action-draw">DRAW</p>
        <p class="action-len">5 CM</p>
        <p class="action-direc">UP</p>
    </div>
</div>
*/