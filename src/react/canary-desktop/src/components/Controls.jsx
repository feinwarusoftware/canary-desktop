import React, { useState } from 'react';
import '../styles/Controls.css'

function Controls(props){
    const playerObject = props.playerObject;

    const [cpos, setpos] = useState(0);
    const [csdata, setcsdata] = useState({
        length:0
    });

    playerObject.setCSPos.connect(function(pos) {
        setpos(pos);
    });

    function setVolume(e){
        playerObject.changeVolume(e.target.value);
    }    

    function setCurrentSongData(len, coverData, artist, album, title) {
        setcsdata({
          cover: coverData,
          length: len,
          albumArtist: artist,
          album: album,
          title: title
        });
    }

    function toMSS(seconds){
        let minutes = Math.floor(seconds / 60);
        let remainingSeconds = seconds - 60 * minutes;
    
        if(remainingSeconds <= 9){
            remainingSeconds = "0" + remainingSeconds;
        }
    
        return minutes + ":" + remainingSeconds;
    }
    
    playerObject.setCSInfo.connect(setCurrentSongData);

    return(
        <div className="playbar">
            <div className="current-song-info">
                <div className="cover-container">
                    <img src={csdata.cover}/>
                </div>
                <div className="nowplaying">
                    <span>{csdata.title}</span>
                    <span>{csdata.albumArtist}</span>
                </div>
            </div>

            <div className="btns">
                <button className="fbtn fas fa-step-backward" onClick={() => playerObject.jump("previous")}></button>
                <button className={props.isPlaying ? 'playbtn fas fa-pause-circle' : 'playbtn fas fa-play-circle'} onClick={props.playCall}></button>
                <button className="fbtn fas fa-step-forward" onClick={() => playerObject.jump("forward")}></button>
            </div>

            <input className="progress-bar" type="range" min="0" value={cpos} 
            onChange={(e)=>{setpos(e.target.value)}}
            style={{backgroundSize:`${cpos * 100 / csdata.length}% 20px`}}
            max={csdata.length} 
            onMouseDown={()=>{playerObject.isDraggingSeekbar = true}} 
            onMouseUp={
                (e)=>{
                    playerObject.seek(e.target.value);
                    playerObject.isDraggingSeekbar = false;
                }
            }/> 
            <div className="time">
                {toMSS(cpos)}/{toMSS(csdata.length)}
            </div>
            <i class="fas fa-volume-up"></i> <input className="vs" type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{setVolume(e)}}/>
        </div>
    );
}

export default Controls;

//MAYBE TODO: use props on App istead of states here (using playerObject as a prop)? I think it's more organized the current way, so I'll keep for now. but maybe change.