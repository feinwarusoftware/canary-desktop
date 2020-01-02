import React, { useState } from 'react';
import Controls from './components/Controls'
import './styles/normalize.css'
import './styles/App.css'

function App(props) {
  //var playerObject;
 // const [playerObject, setPlayerObject] = useState(0);
  const [CSdata, setCSdata] = useState({
    cover:'',
    length:0,
    title:'-',
    albumArtist:'-',
    album:'-'
  });

  const [cpos, setpos] = useState(0);

  const [isPlaying, setPlaying] = useState(false);

  //const [isDragging, setDragging] = useState(false);

  //var isDragging = false;

  const playerObject = props.playerObject;

  function bola(len, coverData, artist, album, title) {
    setCSdata({
      cover: coverData,
      length: len,
      albumArtist: artist,
      album: album,
      title: title
    });
  }

  playerObject.setCSInfo.connect(bola);

  /*playerObject.setCSPos.connect(function(pos) {
    console.log(isDragging);
    if(isDragging === false){
      console.log('to rodando e foda-se')
      setpos(pos);
    }
  });*/

  function playCall(){
    if(isPlaying){
        setPlaying(false)
        playerObject.pause()
    }
    else{
        setPlaying(true)
        playerObject.play()
    }
  }

  function setVolume(e){
    playerObject.changeVolume(e.target.value);
  }

  return (
    <div className="App">
      <div className="bar-placeholder"></div>
      <Controls isPlaying={isPlaying} playCall={playCall} setVolume={setVolume} forward={() => console.log('forward')} back={() => console.log('back')} csdata={{
        cover:CSdata.cover,
        length:CSdata.length,
        title:CSdata.title,
        albumArtist:CSdata.albumArtist,
        album:CSdata.album
      }} playerObject={playerObject} stuff={() => {return playerObject.loadSong(0)}}>
      </Controls>
    </div>
  );
}

export default App;
