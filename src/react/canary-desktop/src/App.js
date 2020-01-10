import React, { useState } from 'react';
import {
  BrowserRouter as Router,
  Route,
  Link
} from "react-router-dom";
import CacheRoute, { CacheSwitch } from 'react-router-cache-route'

import Controls from './components/Controls'
import MusicList from './components/MusicList'
import AlbumPage from './components/AlbumPage'
import './styles/normalize.css'
import './styles/App.css'

function App(props) {
  const location = props.location;

  const [isPlaying, setPlaying] = useState(false);

  /*const [appData, setAppData] = useState({
    location:''
  });*/

  const playerObject = props.playerObject;

  /*function setCurrentSongData(len, coverData, artist, album, title) {
    setCSdata({
      cover: coverData,
      length: len,
      albumArtist: artist,
      album: album,
      title: title
    });
  }

  playerObject.setCSInfo.connect(setCurrentSongData);*/

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

  return (
    <div className="App">
      <Router>
        <div className="menu-bar">
          <Link to="/usermusic">
            <i className="fas fa-music"></i>
          </Link>
        </div>

        <div className="pages">
            <CacheSwitch>
              <CacheRoute exact path="/usermusic" render={(props) => <MusicList type="album" data={playerObject.albums} loc={location} {...props} />} />
              <Route exact path="/album/:id" render={(props) => <AlbumPage playerObject={playerObject} playCall={playCall} />} />
              {/*<Route exact path="/album/:id" render={(props) => <Queue playCall={playCall} playerObject={playerObject} />} />*/}
            </CacheSwitch>
        </div>
      </Router>

      <Controls playerObject={playerObject} isPlaying={isPlaying} playCall={playCall} stuff={() => {return playerObject.loadSong(0)}}>
      </Controls>
    </div>
  );
}

export default App;
