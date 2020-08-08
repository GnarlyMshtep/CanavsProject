//add other keyboard shortcuts to action creation
console.log('client js running :)');

const addActionButt = document.querySelector(".form-add-action-butt");
const launchActionsButt = document.querySelector(".run-actions-butt");
const canvas = document.querySelector(".canvas-display");
canvas.width = 400
canvas.height = 350


const distInp = document.querySelector(".form-cm-input");
const drawBoolInp = document.querySelector(".radio-bttn"); //gets only the first radio  button
const selDirInput = document.querySelector(".form-select-direc");
const actDispBox = document.querySelector(".action-display-container");
const ctx = canvas.getContext('2d');
const canActHei = 7;
const canActWid = 8;
let invalidPosPresent = false;


let canPos = {
    'posX': canvas.offsetWidth / 2,
    'posY': canvas.offsetHeight / 2
};

let lastCanPos = {
    'posX': 0,
    'posY': 0
};

let actions = [];

//event listeners
addActionButt.addEventListener('click', addAction, false);
launchActionsButt.addEventListener('click', launchActions, false);
actDispBox.addEventListener('click', deleteEvent, false)
document.addEventListener('keydown', e => e.code == "KeyA" ? addActionButt.click() : null);
document.addEventListener('keydown', e => e.code == "KeyE" ? launchActionsButt.click() : null);

//able deletion of action

function deleteEvent(e) {
    if (e.target !== e.currentTarget) {
        if (e.target.className === 'delete-action-x') {
            actions.splice(e.target.parentNode.id, 1);
            refreshHtmlActions(refreshCanvasDisplay());
        }
    }
}

function addAction() {
    /*do stuff when action is added 
        1.create action object
        2.display it to the action display
        3.add action to array of all actions
    */
    let action = getFormData() //return an object {distance float, drawBool, direction string}
    console.log(action);
    actions.push(action);
    refreshHtmlActions(refreshCanvasDisplay()); //refresh canvas display return int for number of 
    //actions that did not violate the canvas bounderies.
}

async function launchActions() {
    /*do stuff when actions are added
        1.create the messege
        2.send and print response
        
        not doing whats below
        send the first action
        await confirmation that its done
        send the second
    */
    //alert(stringifyActions(actions));
    const options = {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({
            strData: stringifyActions(actions),
            data: actions
        })
    };

    if (window.confirm("Would you like to send your actions to execution?")) {
        alert("Okay, sending: " + options.body);
        const response = await fetch('/launchActions', options);
        const json = await response.json()
        console.log(json);
    }

}

function stringifyActions(actions) {
    let str = ""
    for (let i = 0; i < actions.length; i++) {
        str += actions[i].stringify();
    }

    return str
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

    ctx.fillStyle = 'blue';
    ctx.lineWidth = 4.5;

    ctx.beginPath();
    ctx.moveTo(canPos.posX, canPos.posY);
}

function refreshCanvasDisplay() {
    initCanvas(); //init canvas to middle and no actions

    for (let i = 0; i < actions.length; i++) { //run thru all actions

        actions[i].moveCtxPos(); //move the canvas position but dont make the change on the canvas quite yes
        console.log(canPos);
        if (canPos.posX < 0 || canPos.posY < 0 || canPos.posX > canvas.offsetWidth || canPos.posY > canvas.offsetHeight) { //validation for too small
            invalidPosPresent = true;
            alert("action number " + i + " has violated the canvas boundery by moving to: (" + canPos.posX + "," + canPos.posY + ").\n Remember, max is (400,350). Action will not be counted to avoid hardware incidets");
            canPos = lastCanPos;
            actions.pop()
            return i
        } else if (actions[i].draw) { //draw if draw
            ctx.lineTo(canPos.posX, canPos.posY);
            console.log("invoked draw");
        } else if (!(actions[i].draw)) { //move if no draw and error
            ctx.moveTo(canPos.posX, canPos.posY);
        }
        invalidPosPresent = false;
        ctx.stroke()
    }
    return actions.length

}

function refreshHtmlActions(numOfActionsNotViolateCanBoundery) {
    actDispBox.innerHTML = ""; //delete everything in the box

    //every time an action is added or popped from
    // actions this should be invoked to redraw the actions\

    for (let i = 0; i < numOfActionsNotViolateCanBoundery; i++) {
        actDispBox.innerHTML += (actions[i].toHtmlString(i));
    }
}

class Action {

    constructor(length0, draw0, direction0) {
        //len in cm draw bool draw or just move marker
        //direction up down left or right
        this.len = length0;
        this.draw = draw0;
        this.direc = direction0;
        this.drawStr = this.draw ? "DRAW" : "NO-DRAW";
    }

    toPost() {

    }

    moveCtxPos() {
        //save last pos in case of accident
        lastCanPos.posX = canPos.posX;
        lastCanPos.posY = canPos.posY;

        // let pxLen = (3 / 100) * window.innerWidth * this.len; below px len for trial version
        let pxLen = this.len * 50
        console.log("pxLen is: " + pxLen);
        switch (this.direc) {
            case "up":
                canPos.posY -= pxLen;
                break;
            case "right":
                canPos.posX += pxLen;
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
        return `<div class="action-box" id="` + String(num) + `">
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

    stringify() {
        return (this.len + "," + this.draw + "," + this.direc + ",").toLowerCase()
    }
}
/*
if ('geolocation' in navigator) {
    console.log('geolocation available');
    navigator.geolocation.getCurrentPosition(position => {
        lat = position.coords.latitude;
        lon = position.coords.longitude;
        console.log(lat, lon);
    });

}*/