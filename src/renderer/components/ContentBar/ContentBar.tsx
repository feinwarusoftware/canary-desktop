import * as React from "react";
import "./ContentBar.scss";

export interface ContentBarProps {}

export interface ContentBarState {}

export default class ContentBar extends React.Component<ContentBarProps, ContentBarState> {
  constructor(props: ContentBarProps) {
    super(props);
  }

  public render() {
    return (
      <div className="contentbar">
        <div className="searchbar">
          <i className="fas fa-search" />
          <input placeholder="Search" />
        </div>
        <div className="content">{this.props.children}</div>
      </div>
    );
  }
}
