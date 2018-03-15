WorkerScript.onMessage = function(msg) {
    console.log(msg.index,msg.prstate);
    msg.model.setProperty(msg.index,"prstate",msg.prstate);

    msg.model.sync();
    msg.model.sync();
 }
