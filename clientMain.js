console.log('client js running :)');

const addActionButt = document.querySelector(".form-add-action-butt");
const launchActionsButt = document.querySelector(".run-actions-butt");
const canvas = document.querySelector(".canvas-display");
const distInp = document.querySelector(".form-cm-input");
const drawBoolInp = document.querySelector(".radio-bttn"); //gets only the first radio  button
const selDirInput = document.querySelector(".form-select-direc");
const actDispBox = document.querySelector(".action-display-container");
const ctx = canvas.getContext('2D');
let canPos = {
    'posX': canvas.clientWidth / 2,
    'posY': canvas.clientHeight / 2
};
initCanvas();

let actions = [];


addActionButt.addEventListener('click', addAction, false);
launchActionsButt.addEventListener('click', launchActions, false);

//musr style and deploy html of action

function addAction() {
    /*do stuff when action is added 
        1.create action object
        2.display it to the action display
        3.add action to array of all actions
    */
    let action = getFormData() //return an object {distance float, drawBool, direction string}
    console.log(action);
    actions.push(action);
    action.addToHtml();
    action.moveOnCanvas()
}



function launchActions() {
    /*do stuff when actions are added
        1.convert actions to HTTP post format
        2.send to the server
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
    ctx.fillStyle = '#9E768F';
    ctx.moveTo(canPos.posX, posY);
}

class Action {

    constructor(length0, draw0, direction0) {
        //len in cm draw bool draw or just move marker
        //direction up down left or right
        this.len = length0;
        this.draw = draw0;
        this.direc = direction0;
    }

    addToHtml() {

    }

    toPost() {

    }

    moveOnCanvas(){
        updateCanPos();
        if(this.draw){
            ctx.LineTo(canvas)
        }
    }

    
}