//component dedicated to functions used in multiple other components

function toMSS(time){
    let seconds = Math.floor(time);
    let minutes = Math.floor(seconds / 60);
    let remainingSeconds = seconds - 60 * minutes;

    if(remainingSeconds <= 9){
        remainingSeconds = "0" + remainingSeconds;
    }

    return minutes + ":" + remainingSeconds;
}

export {toMSS};