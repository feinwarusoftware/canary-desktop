import React from 'react';
import { useLocation, useParams } from "react-router";
import '../styles/AlbumPage.css'

function AlbumPage(props){
    const playerObject = props.playerObject;

    const params = useParams();
    const location = useLocation();
    const data = location.state.data;

    function addAlbumToQueue(currentSong){
        /*data.tracks.map((track, key) => {
            playerObject.insertToQueue(key, track.dir);
        });*/

        playerObject.insertToQueue(0, currentSong.dir);

        playerObject.loadSong(0);
    }

    const tracks = data.tracks.map((track, key) =>
        <div className="track" key={key} onClick={() => {
            playerObject.insertToQueue(0, track.dir);
            playerObject.loadSong(0);
            props.playCall();
        }}>
            <span>{key + 1}</span> {track.title[0]}
        </div>
    );

    return(
        <div className="album-page">
           <img src={process.env.PUBLIC_URL + '/cache/covers/' + params.id + '.png'} />
            <span className="album-name">{data.name}</span>
            <span className="album-artist">{data.artist}</span>
            <span className="album-date">{data.date}</span>
            <div className="tracklist">
                {tracks}
            </div>
        </div>
    );
}

export default AlbumPage;

//MAYBE TODO: use props on App istead of states here (using playerObject as a prop)? I think it's more organized the current way, so I'll keep for now. but maybe change.