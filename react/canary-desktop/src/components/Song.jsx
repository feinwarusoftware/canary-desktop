import React from 'react';
import {toMSS} from './UsefulFunctions';

function Song({songCallback, name, artist, album, number, length, nowPlaying}){
    
    return(
        <div onClick={songCallback} className={`songObj ${nowPlaying ? 'nowPlayingSongObj' : null}`}>
            <span className="songNumber">{number ? number : ""}</span>
            <span className="songArtist">{artist ? artist : ""}</span>
            <span className="songAlbum">{album ? album : ""}</span>
            <span className="songName">{name}</span>
            <span>{toMSS(length)}</span>
        </div>
    );
}

export default Song;