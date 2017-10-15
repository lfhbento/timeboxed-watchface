import { getCurrentVersion } from "./util/util";
import { getText } from "./lang";
import React, { Component, PropTypes } from "react";
import TextField from "./text-field";

class ColorPresets extends Component {
  constructor(props, context) {
    super(props, context);

    this.defaultPresets = {
      "Black and white": {
        bgColor: "#000000",
        hoursColor: "#FFFFFF",
        dateColor: "#FFFFFF",
        altHoursColor: "#FFFFFF",
        batteryColor: "#FFFFFF",
        batteryLowColor: "#FFFFFF",
        bluetoothColor: "#FFFFFF",
        updateColor: "#FFFFFF",
        weatherColor: "#FFFFFF",
        tempColor: "#FFFFFF",
        minColor: "#FFFFFF",
        maxColor: "#FFFFFF",
        stepsColor: "#FFFFFF",
        stepsBehindColor: "#FFFFFF",
        distColor: "#FFFFFF",
        distBehindColor: "#FFFFFF",
        calColor: "#FFFFFF",
        calBehindColor: "#FFFFFF",
        sleepColor: "#FFFFFF",
        sleepBehindColor: "#FFFFFF",
        deepColor: "#FFFFFF",
        deepBehindColor: "#FFFFFF",
        windDirColor: "#FFFFFF",
        windSpeedColor: "#FFFFFF",
        sunriseColor: "#FFFFFF",
        sunsetColor: "#FFFFFF",
        activeColor: "#FFFFFF",
        activeBehindColor: "#FFFFFF",
        heartColor: "#FFFFFF",
        heartColorOff: "#FFFFFF",
        compassColor: "#FFFFFF",
        secondsColor: "#FFFFFF"
      },
      Colorful: {
        bgColor: "#000055",
        hoursColor: "#FFFFFF",
        dateColor: "#AAFFFF",
        altHoursColor: "#00FFFF",
        batteryColor: "#AAAAAA",
        batteryLowColor: "#FF5500",
        bluetoothColor: "#FF5500",
        updateColor: "#00FF00",
        weatherColor: "#FFFF00",
        tempColor: "#FFFF00",
        minColor: "#00FFFF",
        maxColor: "#FF5500",
        stepsColor: "#AAFFFF",
        stepsBehindColor: "#FFFF00",
        distColor: "#AAFFFF",
        distBehindColor: "#FFFF00",
        calColor: "#AAFFFF",
        calBehindColor: "#FFFF00",
        sleepColor: "#AAFFFF",
        sleepBehindColor: "#FFFF00",
        deepColor: "#AAFFFF",
        deepBehindColor: "#FFFF00",
        windDirColor: "#55FF00",
        windSpeedColor: "#55FF00",
        sunriseColor: "#FFFF00",
        sunsetColor: "#FFAA00",
        activeColor: "#AAFFFF",
        activeBehindColor: "#FFFF00",
        heartColor: "#AAFFFF",
        heartColorOff: "#FFFF00",
        compassColor: "#FFFF00",
        secondsColor: "#AAAAAA"
      },
      "Green on black": {
        bgColor: "#000000",
        hoursColor: "#FFFFFF",
        dateColor: "#00FF00",
        altHoursColor: "#00AA00",
        batteryColor: "#AAAAAA",
        batteryLowColor: "#FF5500",
        bluetoothColor: "#FF5500",
        updateColor: "#00FF00",
        weatherColor: "#00FF00",
        tempColor: "#00FF00",
        minColor: "#AAAAAA",
        maxColor: "#00AA00",
        stepsColor: "#00AA00",
        stepsBehindColor: "#AAAAAA",
        distColor: "#00AA00",
        distBehindColor: "#AAAAAA",
        calColor: "#00AA00",
        calBehindColor: "#AAAAAA",
        sleepColor: "#00AA00",
        sleepBehindColor: "#AAAAAA",
        deepColor: "#00AA00",
        deepBehindColor: "#AAAAAA",
        windDirColor: "#AAAAAA",
        windSpeedColor: "#AAAAAA",
        sunriseColor: "#00AA00",
        sunsetColor: "#AAAAAA",
        activeColor: "#00AA00",
        activeBehindColor: "#AAAAAA",
        heartColor: "#00AA00",
        heartColorOff: "#AAAAAA",
        compassColor: "#00AA00",
        secondsColor: "#AAAAAA"
      },
      "Yellow/Orange on black": {
        bgColor: "#000000",
        hoursColor: "#FFFFFF",
        dateColor: "#FFFF00",
        altHoursColor: "#FFAA00",
        batteryColor: "#AAAAAA",
        batteryLowColor: "#FF5500",
        bluetoothColor: "#FF5500",
        updateColor: "#FFFF00",
        weatherColor: "#FFFF00",
        tempColor: "#FFFF00",
        minColor: "#AAAAAA",
        maxColor: "#FFAA00",
        stepsColor: "#FFAA00",
        stepsBehindColor: "#AAAAAA",
        distColor: "#FFAA00",
        distBehindColor: "#AAAAAA",
        calColor: "#FFAA00",
        calBehindColor: "#AAAAAA",
        sleepColor: "#FFAA00",
        sleepBehindColor: "#AAAAAA",
        deepColor: "#FFAA00",
        deepBehindColor: "#AAAAAA",
        windDirColor: "#AAAAAA",
        windSpeedColor: "#AAAAAA",
        sunriseColor: "#FFAA00",
        sunsetColor: "#AAAAAA",
        activeColor: "#FFAA00",
        activeBehindColor: "#AAAAAA",
        heartColor: "#FFAA00",
        heartColorOff: "#AAAAAA",
        compassColor: "#FFAA00",
        secondsColor: "#AAAAAA"
      },
      "Blue on black": {
        bgColor: "#000000",
        hoursColor: "#FFFFFF",
        dateColor: "#00FFFF",
        altHoursColor: "#00AAFF",
        batteryColor: "#AAAAAA",
        batteryLowColor: "#FF5500",
        bluetoothColor: "#FF5500",
        updateColor: "#00FFFF",
        weatherColor: "#00FFFF",
        tempColor: "#00FFFF",
        minColor: "#AAAAAA",
        maxColor: "#00AAFF",
        stepsColor: "#00AAFF",
        stepsBehindColor: "#AAAAAA",
        distColor: "#00AAFF",
        distBehindColor: "#AAAAAA",
        calColor: "#00AAFF",
        calBehindColor: "#AAAAAA",
        sleepColor: "#00AAFF",
        sleepBehindColor: "#AAAAAA",
        deepColor: "#00AAFF",
        deepBehindColor: "#AAAAAA",
        windDirColor: "#AAAAAA",
        windSpeedColor: "#AAAAAA",
        sunriseColor: "#00AAFF",
        sunsetColor: "#AAAAAA",
        activeColor: "#00AAFF",
        activeBehindColor: "#AAAAAA",
        heartColor: "#00AAFF",
        heartColorOff: "#AAAAAA",
        compassColor: "#00AAFF",
        secondsColor: "#AAAAAA"
      },
      "Red on black": {
        bgColor: "#000000",
        hoursColor: "#FFFFFF",
        dateColor: "#FF5555",
        altHoursColor: "#FF0055",
        batteryColor: "#AAAAAA",
        batteryLowColor: "#FFAA00",
        bluetoothColor: "#FFAA00",
        updateColor: "#FF5555",
        weatherColor: "#FF5555",
        tempColor: "#FF5555",
        minColor: "#AAAAAA",
        maxColor: "#FF0055",
        stepsColor: "#FF0055",
        stepsBehindColor: "#AAAAAA",
        distColor: "#FF0055",
        distBehindColor: "#AAAAAA",
        calColor: "#FF0055",
        calBehindColor: "#AAAAAA",
        sleepColor: "#FF0055",
        sleepBehindColor: "#AAAAAA",
        deepColor: "#FF0055",
        deepBehindColor: "#AAAAAA",
        windDirColor: "#AAAAAA",
        windSpeedColor: "#AAAAAA",
        sunriseColor: "#FF0055",
        sunsetColor: "#AAAAAA",
        activeColor: "#FF0055",
        activeBehindColor: "#AAAAAA",
        heartColor: "#FF0055",
        heartColorOff: "#AAAAAA",
        compassColor: "#FF0055",
        secondsColor: "#AAAAAA"
      },
      "Black and white (inverted)": {
        bgColor: "#FFFFFF",
        hoursColor: "#000000",
        dateColor: "#000000",
        altHoursColor: "#000000",
        batteryColor: "#000000",
        batteryLowColor: "#000000",
        bluetoothColor: "#000000",
        updateColor: "#000000",
        weatherColor: "#000000",
        tempColor: "#000000",
        minColor: "#000000",
        maxColor: "#000000",
        stepsColor: "#000000",
        stepsBehindColor: "#000000",
        distColor: "#000000",
        distBehindColor: "#000000",
        calColor: "#000000",
        calBehindColor: "#000000",
        sleepColor: "#000000",
        sleepBehindColor: "#000000",
        deepColor: "#000000",
        deepBehindColor: "#000000",
        windDirColor: "#000000",
        windSpeedColor: "#000000",
        sunriseColor: "#000000",
        sunsetColor: "#000000",
        activeColor: "#000000",
        activeBehindColor: "#000000",
        heartColor: "#000000",
        heartColorOff: "#000000",
        compassColor: "#000000",
        secondsColor: "#000000"
      },
      "Green on white": {
        bgColor: "#FFFFFF",
        hoursColor: "#000000",
        dateColor: "#005500",
        altHoursColor: "#00AA00",
        batteryColor: "#555555",
        batteryLowColor: "#AA0000",
        bluetoothColor: "#AA0000",
        updateColor: "#005500",
        weatherColor: "#005500",
        tempColor: "#005500",
        minColor: "#555555",
        maxColor: "#00AA00",
        stepsColor: "#00AA00",
        stepsBehindColor: "#555555",
        distColor: "#00AA00",
        distBehindColor: "#555555",
        calColor: "#00AA00",
        calBehindColor: "#555555",
        sleepColor: "#00AA00",
        sleepBehindColor: "#555555",
        deepColor: "#00AA00",
        deepBehindColor: "#555555",
        windDirColor: "#555555",
        windSpeedColor: "#555555",
        sunriseColor: "#00AA00",
        sunsetColor: "#555555",
        activeColor: "#00AA00",
        activeBehindColor: "#555555",
        heartColor: "#00AA00",
        heartColorOff: "#555555",
        compassColor: "#00AA00",
        secondsColor: "#555555"
      },
      "Blue on white": {
        bgColor: "#FFFFFF",
        hoursColor: "#000000",
        dateColor: "#005555",
        altHoursColor: "#0055AA",
        batteryColor: "#555555",
        batteryLowColor: "#AA0000",
        bluetoothColor: "#AA0000",
        updateColor: "#005555",
        weatherColor: "#005555",
        tempColor: "#005555",
        minColor: "#555555",
        maxColor: "#0055AA",
        stepsColor: "#0055AA",
        stepsBehindColor: "#555555",
        distColor: "#0055AA",
        distBehindColor: "#555555",
        calColor: "#0055AA",
        calBehindColor: "#555555",
        sleepColor: "#0055AA",
        sleepBehindColor: "#555555",
        deepColor: "#0055AA",
        deepBehindColor: "#555555",
        windDirColor: "#555555",
        windSpeedColor: "#555555",
        sunriseColor: "#0055AA",
        sunsetColor: "#555555",
        activeColor: "#0055AA",
        activeBehindColor: "#555555",
        heartColor: "#0055AA",
        heartColorOff: "#555555",
        compassColor: "#0055AA",
        secondsColor: "#555555"
      }
    };

    this.state = {
      presets: { ...this.defaultPresets, ...props.presets },
      presetName: ""
    };

    this.onAddClick = this.onAddClick.bind(this);
    this.storePresets = this.storePresets.bind(this);
    this._ = getText.bind(this, context.locale);

    this.storePresets();
  }

  componentWillReceiveProps(nextProps) {
    this.setState({
      presets: { ...this.defaultPresets, ...nextProps.presets }
    });
  }

  storePresets() {
    let newPresets = { ...this.state.presets };
    Object.keys(this.defaultPresets).forEach(key => delete newPresets[key]);
    this.props.storePresets(newPresets);
  }

  onClick(preset, e) {
    if (confirm(`Apply preset ${preset}?`)) {
      if (this.props.onSelect) {
        this.props.onSelect(this.state.presets[preset]);
      }
    }
    e.stopPropagation();
  }

  onAddClick(name) {
    if (Object.keys(this.defaultPresets).indexOf(name) !== -1) {
      alert(
        this._(
          "You can't replace default ${name} preset. Choose a different name :)",
          { name }
        )
      );
      return;
    }
    if (Object.keys(this.state.presets).indexOf(name) !== -1) {
      if (
        !confirm(
          this._("This will replace the ${name} preset. Continue?", { name })
        )
      ) {
        return;
      }
    }
    let presets = { ...this.state.presets, ...{ [name]: this.props.colors } };
    this.setState({ presets: presets, presetName: "" });
    window.setTimeout(this.storePresets, 0);
  }

  onRemoveClick(name, e) {
    if (confirm(this._("Remove the preset ${name}?", { name }))) {
      let presets = { ...this.state.presets };
      delete presets[name];
      this.setState({ presets: presets });
      window.setTimeout(this.storePresets, 0);
    }
    e.stopPropagation();
  }

  onTextChange = text => {
    this.setState({
      presetName: text
    });
  };

  render() {
    return (
      <div>
        <div className="list-group">
          {Object.keys(this.state.presets)
            .sort()
            .map(key => {
              return (
                <li
                  className="list-group-item"
                  key={key}
                  onClick={this.onClick.bind(this, key)}
                >
                  {key}
                  {Object.keys(this.defaultPresets).indexOf(key) === -1 ? (
                    <span
                      className="pull-xs-right remove-preset"
                      onClick={this.onRemoveClick.bind(this, key)}
                    >
                      &#x2573;
                    </span>
                  ) : null}
                </li>
              );
            })}
        </div>
        <TextField
          fieldName="presetName"
          buttonLabel={this._("Add New")}
          value={this.state.presetName}
          onButtonClick={this.onAddClick}
          onChange={this.onTextChange}
        />
      </div>
    );
  }
}

ColorPresets.propTypes = {
  onSelect: PropTypes.func,
  colors: PropTypes.object,
  presets: PropTypes.object,
  storePresets: PropTypes.func
};

ColorPresets.defaultProps = {};

ColorPresets.contextTypes = {
  locale: PropTypes.string
};

export default ColorPresets;
