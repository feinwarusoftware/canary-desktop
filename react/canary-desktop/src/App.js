import React from 'react';
import {
  BrowserRouter as Router,
  Route,
  Switch
} from "react-router-dom";
import MusicList from './components/MusicList'
import Controls from './components/Controls'
import Sidebar from './components/Sidebar'
import AlbumPage from './components/AlbumPage'
import Queue from './components/Queue'
import SettingsPage from './components/SettingsPage'
import './styles/App.css';

function App({playerObject, libObject, lastfmObject, location}) {
  return (
    <div className="App">
      <Router>
        <Sidebar></Sidebar>
        <div className="page">
          <Switch>
            <Route exact path="/usermusic" render={(props) => <MusicList libObject={libObject} />} />
            <Route exact path="/album/:id" render={(props) => <AlbumPage playerObject={playerObject} location={location} {...props}/> }  />
            <Route exact path="/queue" render={(props) => <Queue playerObject={playerObject} {...props} />} />
            <Route exact path="/settings" render={(props) => <SettingsPage lastfmObject={lastfmObject} {...props} />} />
          </Switch>
        </div>
      </Router>
      <Controls playerObject={playerObject} libObject={libObject}></Controls>
    </div>
  );
}

export default App;
