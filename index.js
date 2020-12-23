const express = require('express');
const os = require('os');
/*find Ipv4 adress to host server at*/
let ifaces = os.networkInterfaces();
let localIP;
Object.keys(ifaces).forEach(function (ifname) {
    var alias = 0;

    ifaces[ifname].forEach(function (iface) {
        if ('IPv4' !== iface.family || iface.internal !== false) {
            // skip over internal (i.e. 127.0.0.1) and non-ipv4 addresses
            return;
        }
        localIP = iface.address;
    });

});

let app = express()
//app.listen(3000, () => console.log("listening at 3000"))
app.listen(3000, localIP, () => console.log("listening at: \nhttp://" + localIP + ":3000"));
/*app.listen('80', '192.168.0.33', () => {
    console.info(`server started on port 80)`);
});*/

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
        length: request.body.length,
        strData: request.body.strData,
        data: request.body.data,
        used: false
    });

    /*fs.readFileSync('savedData.json', function (err, result) {
        var json = JSON.parse(result)
        json.push()

        fs.writeFileSync("savedData.json", json + JSON.stringify(data));

        response.json({
            status: "success!",
            timeStamp: String(d),
            data: request.body.strData,
            index: data.length
        });
    });*/
});

//ARDUINO GET REQUEST
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
    return "null"
}