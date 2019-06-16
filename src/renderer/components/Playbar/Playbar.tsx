import * as React from "react";
import "./Playbar.scss";

export interface PlaybarProps {}

export interface PlaybarState {}

export default class Playbar extends React.Component<PlaybarProps, PlaybarState> {
  constructor(props: PlaybarProps) {
    super(props);
  }

  public render() {
    return (
      <div className="playbar">
        <div className="left">
          <i className="fas fa-fast-backward" />
          <i className="fas fa-play" />
          <i className="fas fa-fast-forward" />

          <div className="volume">
            <i className="fas fa-volume-off" />
            <div className="volume-back" />
            <div className="slider" />
          </div>
        </div>
        <div className="right">
          <i className="fas fa-sliders-v" />
          <i className="fas fa-random" />
          <i className="fas fa-repeat" />
        </div>
      </div>
    );
  }
}
