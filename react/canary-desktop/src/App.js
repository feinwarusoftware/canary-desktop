import React from 'react';
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
  const playerObject = props.playerObject;

  function playCall(){
    playerObject.playerClass.playing().then((isPlaying)=>{
      if(isPlaying){
        playerObject.playerClass.pause()
      }
      else{
        playerObject.playerClass.play()
      }
    })
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
      <Controls playerObject={playerObject} playCall={playCall} stuff={() => {
        playerObject.insertToQueue(0, "G:/Músicas/Fallout 76_ Take Me Home, Country Roads/01 Fallout 76_ Take Me Home, Country.m4a"); 
        playerObject.loadSong(0);
        }}></Controls>
    </div>
  );
}

export default App;
