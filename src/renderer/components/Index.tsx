import { hot } from "react-hot-loader/root";
import * as React from "react";
import Sidebar from "./Sidebar/Sidebar";
import Playbar from "./Playbar/Playbar";
import ContentBar from "./ContentBar/ContentBar";
import ImageHeader from "./ImageHeader/ImageHeader";
import "./app.scss";

const Index = () => (
  <div className="default-dark">
    <div id="root">
      <Sidebar />
      <ContentBar />
      <div id="main-content">
        <ImageHeader
          title="Dream Theater"
          image="https://www.rollingstone.com/wp-content/uploads/2018/11/Dream-Theater-2018-Mark-Maryanovich-.jpg"
        />
      </div>
    </div>
    <Playbar />
  </div>
);

export default hot(Index);
