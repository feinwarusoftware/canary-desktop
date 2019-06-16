import * as React from "react";
import "./ImageHeader.scss";

export interface ImageHeaderProps {
  title: string;
  image: string;
}

export interface ImageHeaderState {}

export default class ImageHeader extends React.Component<ImageHeaderProps, ImageHeaderState> {
  constructor(props: ImageHeaderProps) {
    super(props);
  }

  public render() {
    return (
      <div className="imageheader">
        <div className="image">
          <div className="gradient" />
          <img src={this.props.image} />
        </div>
        <h1>{this.props.title}</h1>
      </div>
    );
  }
}
