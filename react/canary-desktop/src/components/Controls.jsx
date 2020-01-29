import React, { useState } from 'react';
import '../styles/Controls.css'

function Controls(props){
    const playerObject = props.playerObject;

    const [cpos, setpos] = useState(0);
    const [nowPlayingData, setNowPlayingData] = useState({
        length:0
    });

    playerObject.setNowPlayingPos.connect(function(pos) {
        setpos(pos); //possible bug: the DevTools console shows this is being called MANY times a half-second. believe it's just a debug glitch.
    });

    function setVolume(e){
        playerObject.playerClass.changeVolume(e.target.value);
    }    

    function setCurrentSongData(len, coverData, artist, album, title) {
        setNowPlayingData({
          cover: coverData,
          length: len,
          albumArtist: artist,
          album: album,
          title: title
        });
    }

    function toMSS(time){
        let seconds = Math.floor(time);
        let minutes = Math.floor(seconds / 60);
        let remainingSeconds = seconds - 60 * minutes;
    
        if(remainingSeconds <= 9){
            remainingSeconds = "0" + remainingSeconds;
        }
    
        return minutes + ":" + remainingSeconds;
    }
    
    playerObject.setNowPlayingInfo.connect(setCurrentSongData);

    return(
        <div className="playbar">
            <button onClick={props.stuff}>Load Test Song</button>
            <div className="current-song-info">
                <div className="cover-container">
                    <img alt="Current Song Album Cover" src={nowPlayingData.cover} />
                </div>
                <div className="nowplaying">
                    <span>{nowPlayingData.title}</span>
                    <span>{nowPlayingData.albumArtist}</span>
                </div>
            </div>

            <div className="btns">
                <button onClick={() => playerObject.jump("previous")}>Previous</button>
                <button onClick={props.playCall}>Play/Pause</button>
                <button onClick={() => playerObject.jump("forward")}>Forward</button>
            </div>

            <input className="progress-bar" type="range" min="0" value={cpos} 
            onChange={(e)=>{setpos(e.target.value)}}
            style={{backgroundSize:`${cpos * 100 / nowPlayingData.length}% 20px`}}
            max={nowPlayingData.length} 
            onMouseDown={()=>{playerObject.isDraggingSeekbar = true}} 
            onMouseUp={
                (e)=>{
                    playerObject.playerClass.seek(e.target.value);
                    playerObject.isDraggingSeekbar = false;
                }
            }/> 
            <div className="time">
                {toMSS(cpos)}/{toMSS(nowPlayingData.length)}
            </div>
            <i className="fas fa-volume-up"></i> <input className="vs" type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{setVolume(e)}}/>
        </div>
    );
}

export default Controls;