const express = require('express');
const fs = require('fs')

let app = express()
app.listen(3000, () => console.log("listening at 3000"))
//server.listen(3000, 'your ipv4 address', () => console.log("listening!"));

app.use(express.static('webClient'));
app.use(express.json({
    limit: "1mb"
}));


let data = []

app.post('/launchActions', function (request, response) {
    console.log('I got a request!');
    console.log(request.body.strData);

    let d = new Date()

    data.push({
        timeStamp: String(d),
        strData: request.body.strData,
        data: request.body.data,
        used: false
    });

    fs.writeFileSync("savedData.json", JSON.stringify(data));

    response.json({
        status: "success!",
        timeStamp: String(d),
        data: request.body.strData,
        index: data.length
    })
});

app.get('/getAction', function (request, response) {
    response.send(getLatestUnusedResponse()); //send strData of latest unused action
});

function getLatestUnusedResponse() { //return strData of latest unused action
    for (let i = 0; i < data.length; i++) {
        if (data[i].used == false) {
            data[i].used = true;
            return data[i].strData
        }
    }
}