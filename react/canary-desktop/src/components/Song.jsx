import React from 'react';
import {toMSS} from './UsefulFunctions';

function Song({songCallback, name, number, length, nowPlaying}){
    
    return(
        <div onClick={songCallback}>
            <span>{number ? number : ""}</span>
            <span>{name}</span>
            <span>{toMSS(length)}</span>
            <span>{nowPlaying ? "tocando" : ""}</span>
        </div>
    );
}

export default Song;