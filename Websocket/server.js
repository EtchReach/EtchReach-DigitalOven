const app = require('express')();
const appWs = require('express-ws')(app);
const path = require('path');

app.ws('/echo', ws => {
    ws.on('message', msg => {
        console.log('Received: ', msg);
        ws.send(msg);
    });
    app.get('/',(req,res) => {
        res.sendFile(path.join(__dirname,'/index.html'));
    })
    app.post('/light',(req,res) => {
        ws.send('Hello World!');
    });
});

port = process.env.PORT || 5000;

app.listen(port, () => console.log(`Listening on port ${port}`));