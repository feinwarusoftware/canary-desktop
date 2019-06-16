import * as React from "react";
import "./Sidebar.scss";

export interface SidebarProps {}

export interface SidebarState {}

export default class Sidebar extends React.Component<SidebarProps, SidebarState> {
  constructor(props: SidebarProps) {
    super(props);
  }

  public render() {
    return (
      <div className="sidebar">
        <i className="fas fa-user active">
          <span className="tooltiptext">Artists</span>
        </i>
        <i className="fas fa-compact-disc">
          <span className="tooltiptext">Albums</span>
        </i>
        <i className="fas fa-music">
          <span className="tooltiptext">Songs</span>
        </i>
        <i className="fas fa-list">
          <span className="tooltiptext">Playlists</span>
        </i>
        <i className="fas fa-rss">
          <span className="tooltiptext">Podcasts</span>
        </i>
        <i className="fas fa-plug">
          <span className="tooltiptext">Plugins</span>
        </i>

        <div className="bottom-settings">
          <i className="fas fa-pencil-ruler">
            <span className="tooltiptext">Customize</span>
          </i>
          <i className="fas fa-cog">
            <span className="tooltiptext">Settings</span>
          </i>
        </div>
      </div>
    );
  }
}
