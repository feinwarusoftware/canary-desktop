import React, { useState } from 'react';
import '../styles/Controls.css'

function Controls(props){
    const [cpos, setpos] = useState(0);

    props.playerObject.setCSPos.connect(function(pos) {
        setpos(pos);
    });

    return(
        <div className="playbar">
            <div className="current-song-info">
                <div className="cover-container">
                    <img src={props.csdata.cover}/>
                </div>
                <span>{props.csdata.title}</span>
                <span>{props.csdata.albumArtist}</span>
            </div>

            <div className="btns">
                <button className={props.isPlaying ? 'playbtn fas fa-pause-circle' : 'playbtn fas fa-play-circle'} onClick={props.playCall}></button>
            </div>

            <input type="range" min="0" value={cpos} 
            onChange={(e)=>{setpos(e.target.value)}} 
            max={props.csdata.length} 
            onMouseDown={()=>{props.playerObject.isDraggingSeekbar = true}} 
            onMouseUp={
                (e)=>{
                    props.playerObject.seek(e.target.value);
                    props.playerObject.isDraggingSeekbar = false;
                }
            }/> 
            Length: {cpos}/{props.csdata.length}
            Volume: <input type="range" defaultValue="1" min="0" max="1" step="any" onChange={(e)=>{props.setVolume(e)}}/>
            <button onClick={props.stuff}>Magic Button</button>
        </div>
    );
}

export default Controls;

//MAYBE TODO: use props on App istead of states here (using playerObject as a prop)? I think it's more organized the current way, so I'll keep for now. but maybe change.