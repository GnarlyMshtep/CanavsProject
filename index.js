const express = require('express');

let app = express()
app.listen(3000, () => console.log("listening at 3000"))

app.use(express.static('webClient'));
app.use(express.json({
    limit: "1mb"
}));

app.post('/launchActions', function (request, response) {
    console.log('I got a request!');
    console.log(request.body.strData);
});