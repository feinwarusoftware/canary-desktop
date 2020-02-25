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

function App({playerObject, libObject, location}) {
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
            <Route exact path="/usermusic" render={(props) => <MusicList libObject={libObject} />} />
            <Route exact path="/album/:id" render={(props) => <AlbumPage playerObject={playerObject} location={location} {...props}/> }  />
            <Route exact path="/queue" render={(props) => <Queue playerObject={playerObject} {...props} />} />
          </Switch>
        </div>
      </Router>
      <Controls playerObject={playerObject} playCall={playCall} stuff={() => {
        playerObject.insertToQueue(0, "G:/Músicas/Toehider/Covers/16032018 - cover 4 GRAVEMAKERS AND GUNSLINGERS.wav"); 
        //playerObject.insertToQueue(1, "G:/Músicas/Dream Theater/Falling Into Infinity/07. Lines in the Sand.flac");
        playerObject.playerClass.loadSong(0);
        }}></Controls>
    </div>
  );
}

export default App;
