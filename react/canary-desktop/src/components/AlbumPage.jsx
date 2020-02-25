import React from 'react';
import { useParams } from "react-router";
import '../styles/AlbumPage.css'

function AlbumPage({location, playerObject}){
    const params = useParams();
    const data = location.state.data;
    console.log(data);

    function insertAlbum(pos){
        data.tracks.forEach(function(song, index){
            //TODO: looping for inserting terrible, optimze by loading array (possible!)
            playerObject.insertToQueue(index, song);
        });
        //playerObject.playerClass.loadSong(pos);
    }

    const tracks = data.tracks.map((song, index) =>
        <li 
        key={index}
        onClick={()=>{
            playerObject.insertAlbumToQueue(data.tracks, index);
        }}
        >{index + 1} {song.title[0]}</li>
    );

    return(
        <div className="album">
            <img src={process.env.PUBLIC_URL + `/cache/covers/${params.id}.jpg`} />
            <br></br>
            {data.name}
            <br></br>
            by {data.artist}
            <br></br>
            {tracks}
        </div>
    );
}

export default AlbumPage;