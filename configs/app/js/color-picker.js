import Field from "./field";
import React, { Component, PropTypes } from "react";
import Swatches from "./swatches";

class ColorPicker extends Component {
  constructor(props) {
    super(props);
    this.state = {
      color: props.color,
      secondColor: props.secondColor,
      panelVisible: false,
      secondPanelVisible: false
    };
    this.onColorChange = this.onColorChange.bind(this);
    this.onSecondColorChange = this.onSecondColorChange.bind(this);
    this.showPanel = this.showPanel.bind(this);
    this.togglePanel = this.togglePanel.bind(this);
    this.toggleSecondPanel = this.toggleSecondPanel.bind(this);
    this.hidePanel = this.hidePanel.bind(this);
  }

  onColorChange(color) {
    this.setState({ color: color });
    if (this.props.onChange) {
      this.props.onChange(color);
    }
    this.hidePanel();
  }

  onSecondColorChange(color) {
    this.setState({ secondColor: color });
    if (this.props.onSecondColorChange) {
      this.props.onSecondColorChange(color);
    }
    this.hidePanel();
  }

  showPanel() {
    this.setState({ panelVisible: true });
  }

  hidePanel() {
    this.setState({ panelVisible: false, secondPanelVisible: false });
  }

  togglePanel() {
    this.setState({ panelVisible: !this.state.panelVisible });
  }

  toggleSecondPanel() {
    this.setState({ secondPanelVisible: !this.state.secondPanelVisible });
  }

  shouldComponentUpdate(nextProps, nextState) {
    return this.state !== nextState;
  }

  componentWillReceiveProps(nextProps) {
    if (
      this.state.color !== nextProps.color ||
      this.state.secondColor !== nextProps.secondColor
    ) {
      this.setState({
        color: nextProps.color,
        secondColor: nextProps.secondColor
      });
    }
  }

  render() {
    return (
      <div className="color-picker">
        <Field
          fieldName={this.props.fieldName}
          label={this.props.label}
          longLabel={true}
        >
          <div className="swatch" onClick={this.togglePanel}>
            <div
              className="swatch--color"
              style={{ backgroundColor: this.state.color }}
            />
          </div>
          {this.state.secondColor ? (
            <div className="swatch" onClick={this.toggleSecondPanel}>
              <div
                className="swatch--color"
                style={{ backgroundColor: this.state.secondColor }}
              />
            </div>
          ) : null}
        </Field>
        {this.state.panelVisible || this.state.secondPanelVisible ? (
          <div className="color-panel">
            <Swatches
              onColorChange={
                this.state.panelVisible
                  ? this.onColorChange
                  : this.onSecondColorChange
              }
            />
            <div className="color-panel--shim" onClick={this.hidePanel} />
          </div>
        ) : null}
      </div>
    );
  }
}

ColorPicker.propTypes = {
  color: PropTypes.string,
  secondColor: PropTypes.string,
  onChange: PropTypes.func,
  onSecondColorChange: PropTypes.func,
  fieldName: PropTypes.string,
  label: PropTypes.string
};

ColorPicker.defaultProps = {};

export default ColorPicker;
