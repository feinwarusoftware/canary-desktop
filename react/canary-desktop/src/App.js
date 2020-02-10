import React, { useState } from 'react';
import {
  BrowserRouter as Router,
  Route,
  Link,
  Switch
} from "react-router-dom";
import MusicList from './components/MusicList'
import Controls from './components/Controls'
import Sidebar from './components/Sidebar'
import AlbumPage from './components/AlbumPage'
import Queue from './components/Queue'
import './styles/App.css';

function App(props) {
  const [isPlaying, setPlaying] = useState(false);
  const playerObject = props.playerObject;

  function playCall(){
    if(isPlaying){
        setPlaying(false)
        playerObject.playerClass.pause()
    }
    else{
        setPlaying(true)
        playerObject.playerClass.play()
    }
  }

  return (
    <div className="App">
      <Router>
        <Sidebar></Sidebar>
        <div className="page">
          <Switch>
            <Route exact path="/usermusic" render={(props) => <MusicList />} />
            <Route exact path="/album/:id" render={(props) => <AlbumPage />} />
            <Route exact path="/queue" render={(props) => <Queue />} />
          </Switch>
        </div>
      </Router>
      <Controls playerObject={playerObject} isPlaying={isPlaying} playCall={playCall} stuff={() => {
        playerObject.insertToQueue(0, "G:/Músicas/Dream Theater/Six Degrees Of Inner Turbulence/CD2/01. Six Degrees of Inner Turbulence_ I. Overture.flac"); 
        playerObject.insertToQueue(1, "G:/Músicas/Dream Theater/Six Degrees Of Inner Turbulence/CD2/02. Six Degrees of Inner Turbulence_ II. About to Crash.flac")
        playerObject.loadSong(0);
        //playerObject.playerClass.seek(406);
        }}></Controls>
    </div>
  );
}

export default App;
