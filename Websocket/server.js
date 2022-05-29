const express = require('express');
const app = express();
const appWs = require('express-ws')(app);
const path = require('path');
const bodyParser = require('body-parser');

app.use(bodyParser.urlencoded({ extended: true }));
app.use((req, res, next) => {
    res.header("Access-Control-Allow-Origin", "*");
    res.header("Access-Control-Allow-Headers", "Origin, X-Requested-With, Content-Type, Accept");
    next();
});
app.use(express.json());

app.ws('/echo', ws => {
    ws.on('message', msg => {
        console.log('Received: ', msg);
        ws.send(msg);
    });
    app.get('/',(req,res) => {
        res.json({status:'success',method:'get'})
    });
    app.post('/api',(req,res) => {
        console.log('Got body:', req.body);
        res.json({status:'success',method:'post', ...req.body});
    });
});



port = process.env.PORT || 5000;

app.listen(port, () => console.log(`Listening on port ${port}`));