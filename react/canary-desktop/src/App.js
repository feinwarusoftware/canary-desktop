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
      <Controls playerObject={playerObject} isPlaying={isPlaying} playCall={playCall} stuff={() => {return playerObject.playerClass.loadSong(0)}}></Controls>
    </div>
  );
}

export default App;
