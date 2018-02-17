/* eslint-disable sort-imports-es6-autofix/sort-imports-es6 */
import objectAssign from 'object-assign';
Object.assign = Object.assign || objectAssign;

import Dropdown from 'react-dropdown';
import FastClick from 'react-fastclick-alt';
import LZString from './util/lz-string';
import React, { Component, PropTypes } from 'react'; //eslint-disable-line react/no-deprecated
import ReactDOM from 'react-dom';
import Toggle from 'react-toggle';
import classnames from 'classnames';
import fetch from 'isomorphic-fetch';

import 'react-dropdown/style.css';
import 'react-toggle/style.css';
import '../css/main.scss';
/* eslint-enable sort-imports-es6-autofix/sort-imports-es6 */

/**
 * Main layout component.
 *
 * @@:Layout
 *
 * @type {Layout}
 */
class Layout extends Component {
    constructor(props) {
        super(props);
        this.currentVersion = getCurrentVersion();
        this.platform = getPlatform();

        this.defaultState = { ...defaultState };

        this.defaultColors = { ...defaultColors };

        this.ignoreKeys = ['showDebug'];

        this.colorKeys = Object.keys(this.defaultColors);

        let newState = this.filterValidKeys(this.props.state, [...this.colorKeys, ...Object.keys(this.defaultState)]);

        this.moduleStateKeys = ['slotA', 'slotB', 'slotC', 'slotD', 'slotE', 'slotF'];
        this.moduleSleepStateKeys = this.moduleStateKeys.map(
            (key) => `sleep${key.slice(0, 1).toUpperCase()}${key.slice(1)}`
        );
        this.moduleTapStateKeys = this.moduleStateKeys.map((key) => `tap${key.slice(0, 1).toUpperCase()}${key.slice(1)}`);
        this.moduleWristStateKeys = this.moduleStateKeys.map(
            (key) => `wrist${key.slice(0, 1).toUpperCase()}${key.slice(1)}`
        );

        this.state = { ...this.defaultState, ...this.defaultColors, ...newState };

        this.modulesAll = [
            { value: '0', label: 'None' },
            { value: '1', label: 'Current Weather' },
            { value: '2', label: 'Min/Max Temp' },
            { value: '3', label: 'Steps' },
            { value: '4', label: 'Distance' },
            { value: '5', label: 'Calories' },
            { value: '6', label: 'Sleep Time' },
            { value: '7', label: 'Deep Sleep Time' },
            { value: '8', label: 'Wind dir./speed' },
            { value: '11', label: 'Sunrise' },
            { value: '12', label: 'Sunset' },
            { value: '13', label: 'Active time' },
            { value: '14', label: 'Heart rate' },
            ...(this.platform !== 'diorite' ? [{ value: '15', label: 'Compass' }] : []),
            { value: '16', label: 'Seconds' },
            { value: '17', label: 'Battery level' },
            { value: '20', label: 'Cryptocurrency A' },
            { value: '21', label: 'Cryptocurrency B' },
            { value: '22', label: 'Cryptocurrency C' },
            { value: '23', label: 'Cryptocurrency D' },
        ];

        this.modulesAplite = this.modulesAll.filter(
            (module) => ['0', '1', '2', '8', '11', '12', '15', '16', '17'].indexOf(module.value) !== -1
        );

        this.textModulesAll = [
            ...this.modulesAll.filter(
                (module) =>
                    ['0', '3', '4', '5', '6', '7', '13', '16', '17', '20', '21', '22', '23'].indexOf(module.value) !==
                    -1
            ),
            { value: '18', label: 'Alternate time zone A' },
            { value: '19', label: 'Alternate time zone B' },
        ].sort((a, b) => parseInt(a.value, 10) - parseInt(b.value, 10));

        this.textModulesAplite = this.textModulesAll.filter(
            (module) => ['0', '16', '17', '18'].indexOf(module.value) !== -1
        );

        this.timezones = allTimezones;
        this.locales = allLocales;
        this.fonts = allFonts;
        this.dateFormatOptions = allDateOptions;

        this.weatherModules = ['1', '2', '8', '11', '12'];
        this.healthModules = ['3', '4', '5', '6', '7', '13', '14'];

        this.textModules = this.platform === 'aplite' ? this.textModulesAplite : this.textModulesAll;
        this.modules = this.platform === 'aplite' ? this.modulesAplite : this.modulesAll;
    }

    filterValidKeys = (data, keys, invert = false) => {
        let newData = { ...data };
        Object.keys(newData).map((key) => {
            if ((!invert && keys.indexOf(key) === -1) || (invert && keys.indexOf(key) !== -1)) {
                delete newData[key];
            }
        });
        return newData;
    };

    wipeConfigs = () => {
        if (
            window.confirm(
                'This will clean all the config data for Timeboxed saved on your phone. ' +
                    'This also wipes any custom presets, manual location and API keys you might have entered. ' +
                    'Do you want to proceed?'
            )
        ) {
            this.setState({ ...this.defaultState, ...this.defaultColors });
        }
    };

    storePresets = (newPresets) => {
        this.setState({
            presets: newPresets,
        });
    };

    onChange = (key, value) => {
        this.setState({ [key]: value });
    };

    onChangeDropdown = (key, value) => {
        let newValue = value ? value.value : null;
        this.onChange(key, newValue);
    };

    onPresetSelect = (colors) => {
        this.setState(colors);
    };

    toggleDebug = () => {
        let showDebug = this.state.showDebug;
        this.setState({ showDebug: !showDebug });
    };

    onSubmit = () => {
        if (
            (this.isProviderSelected('0') && !this.state.openWeatherKey) ||
            (this.isProviderSelected('1') && !this.state.weatherKey) ||
            (this.isProviderSelected('3') && !this.state.forecastKey)
        ) {
            alert('Please enter a valid API key for the selected provider');
            return;
        }

        if (this.isEnabled(['20']) && (!this.state.cryptoTo || !this.state.cryptoFrom)) {
            alert('Please enter currencies for Cryptocurrency A');
            return;
        }

        if (this.isEnabled(['21']) && (!this.state.cryptoToB || !this.state.cryptoFromB)) {
            alert('Please enter currencies for Cryptocurrency B');
            return;
        }

        if (this.isEnabled(['22']) && (!this.state.cryptoToC || !this.state.cryptoFromC)) {
            alert('Please enter currencies for Cryptocurrency C');
            return;
        }

        if (this.isEnabled(['23']) && (!this.state.cryptoToD || !this.state.cryptoFromD)) {
            alert('Please enter currencies for Cryptocurrency D');
            return;
        }

        if (this.state.heartHigh && isNaN(this.state.heartHigh)) {
            alert('Value for high heart rate limit should be a number');
            return;
        }

        if (this.state.heartLow && isNaN(this.state.heartLow)) {
            alert('Value for low heart rate limit should be a number');
            return;
        }

        if (this.props.onSubmit) {
            this.props.onSubmit({
                ...this.filterValidKeys(this.state, this.ignoreKeys, true),
            });
        }
    };

    getCurrentColors = () => {
        return this.colorKeys.reduce(
            (colors, colorKey) => ({
                ...colors,
                ...{ [colorKey]: this.state[colorKey] },
            }),
            {}
        );
    };

    isWeatherEnabled = () => {
        return this.isEnabled(this.weatherModules);
    };

    isHealthEnabled = () => {
        return this.isEnabled(this.healthModules) || this.state.showSleep;
    };

    isEnabled = (moduleIndexes) => {
        return (
            this.moduleStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1) ||
            (this.state.showSleep &&
                this.moduleSleepStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1)) ||
            (this.state.showTap &&
                this.moduleTapStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1)) ||
            (this.state.showWrist &&
                this.moduleWristStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1))
        );
    };

    isEnabledTapWrist = (moduleIndexes) => {
        return (
            (this.state.showTap &&
                this.moduleTapStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1)) ||
            (this.state.showWrist &&
                this.moduleWristStateKeys.some((key) => moduleIndexes.indexOf(this.state[key]) !== -1))
        );
    };

    isProviderSelected = (index) => {
        return [index].indexOf(this.state.weatherProvider) !== -1;
    };

    verifyLocation = (loc) => {
        console.log(loc);
        verifyLocation(loc, this.state.weatherProvider, this.state.weatherKey);
    };

    getMasterKeyData = () => {
        console.log(`Fetching data for ${this.state.masterKeyEmail}`);
        fetchMasterKeyData(this.state.masterKeyEmail, this.state.masterKeyPin, (keys) => {
            console.log(`WU: ${keys.wu} / DarkSky ${keys.forecast}`);
            alert('Keys retrieved successfully!');
            this.setState({
                weatherKey: keys.wu || '',
                forecastKey: keys.forecast || '',
            });
        });
    };

    getModules = (options, mode, type) => {
        return (
            <div>
                {options.map((item) => {
                    if (Array.isArray(item)) {
                        return (
                            <SideBySideFields>
                                {this.renderModuleDropdown(item[0], mode)}
                                {this.renderModuleDropdown(item[1], mode)}
                            </SideBySideFields>
                        );
                    } else {
                        return this.renderModuleDropdown(item, mode);
                    }
                })}
            </div>
        );
    };

    renderModuleDropdown = (item, mode) => {
        return (
            <DropdownField
                fieldName={mode ? `${item.name}-${mode}` : item.name}
                label={item.label}
                options={item.textOnly ? this.textModules : this.modules}
                searchable={false}
                clearable={false}
                labelPos={item.labelPos}
                selectedItem={
                    this.state[mode ? `${mode}${item.slot.slice(0, 1).toUpperCase()}${item.slot.slice(1)}` : item.slot]
                }
                onChange={this.onChangeDropdown.bind(
                    this,
                    mode ? `${mode}${item.slot.slice(0, 1).toUpperCase()}${item.slot.slice(1)}` : item.slot
                )}
            />
        );
    };

    getEnabledModules = () => {
        let state = this.state;

        let modules = {
            Default: this.getModules(baseModules),
        };

        if (state.showSleep) {
            modules['Sleep'] = this.getModules(baseModules, 'sleep');
        }

        if (state.showTap) {
            modules['Tap'] = this.getModules(baseModules, 'tap');
        }

        if (state.showWrist) {
            modules['Shake'] = this.getModules(baseModules, 'wrist');
        }

        return modules;
    };

    getEnabledModulesRound = () => {
        let state = this.state;

        let modules = {
            Default: this.getModules(baseModulesRound),
        };

        if (state.showSleep) {
            modules['Sleep'] = this.getModules(baseModulesRound, 'sleep');
        }

        if (state.showTap) {
            modules['Tap'] = this.getModules(baseModulesRound, 'tap');
        }

        if (state.showWrist) {
            modules['Shake'] = this.getModules(baseModulesRound, 'wrist');
        }

        return modules;
    };

    renderCryptocurrency = (type) => {
        type = type || '';
        return (
            <div className="card card--custom">
                <div className="card-header subtitle">{`Cryptocurrency ${type || 'A'}`}</div>
                <DropdownField
                    fieldName={`cryptoMarket${type}`}
                    label={'Market'}
                    options={cryptoMarkets}
                    searchable={false}
                    clearable={false}
                    selectedItem={this.state[`cryptoMarket${type}`]}
                    onChange={this.onChangeDropdown.bind(this, `cryptoMarket${type}`)}
                />
                <SideBySideFields align={false}>
                    <TextField
                        fieldName={`cryptoFrom${type}`}
                        label="Currency"
                        value={this.state[`cryptoFrom${type}`]}
                        onChange={(value) => this.onChange(`cryptoFrom${type}`, (value || '').toUpperCase().trim())}
                        placeholder="BTC"
                        labelPos="top"
                    />
                    <TextField
                        fieldName={`cryptoTo${type}`}
                        label="Show price in"
                        value={this.state[`cryptoTo${type}`]}
                        onChange={(value) => this.onChange(`cryptoTo${type}`, (value || '').toUpperCase().trim())}
                        placeholder="USD"
                        labelPos="top"
                    />
                </SideBySideFields>
                <ToggleField
                    fieldName={`cryptoMulti${type}`}
                    label={'Multiply value by 10^8 (display in Satoshis)'}
                    checked={this.state[`cryptoMulti${type}`]}
                    onChange={this.onChange.bind(this, `cryptoMulti${type}`)}
                />
            </div>
        );
    };

    renderAlternateTimezone = (type) => {
        type = type || '';
        return (
            <DropdownField
                fieldName={`timezones${type}`}
                label={`Additional Timezone ${type || 'A'}`}
                options={this.timezones}
                searchable={true}
                clearable={false}
                selectedItem={this.state[`timezones${type}`]}
                onChange={this.onChangeDropdown.bind(this, `timezones${type}`)}
                labelPos="top"
            />
        );
    };

    render() {
        let state = this.state;
        return (
            <div>
                <h1 className="title">Timeboxed</h1>
                <VersionIndicator onClick={this.toggleDebug} />

                {state.showDebug && (
                    <OptionGroup title={'Debug'}>
                        <ul className="debug-list">
                            {Object.keys(this.state).map((key) => {
                                return (
                                    <li key={key}>
                                        {key}: {JSON.stringify(this.state[key])}
                                    </li>
                                );
                            })}
                        </ul>
                        <button className="btn btn-danger" onClick={this.wipeConfigs}>
                            Reset configs
                        </button>
                    </OptionGroup>
                )}

                <OptionGroup title={'General'}>
                    {this.platform !== 'chalk' && (
                        <RadioButtonGroup
                            fieldName="textAlign"
                            label="Text Align"
                            options={[
                                { value: '0', label: 'Left' },
                                { value: '1', label: 'Center' },
                                { value: '2', label: 'Right' },
                            ]}
                            selectedItem={state.textAlign}
                            onChange={this.onChange.bind(this, 'textAlign')}
                        />
                    )}
                    <DropdownField
                        fieldName="fontType"
                        label="Font"
                        options={this.fonts}
                        selectedItem={state.fontType}
                        onChange={this.onChangeDropdown.bind(this, 'fontType')}
                    />
                    <ToggleField
                        fieldName="leadingZero"
                        label={'Hours with leading zero'}
                        checked={state.leadingZero}
                        onChange={this.onChange.bind(this, 'leadingZero')}
                    />
                    <ToggleField
                        fieldName="bluetoothDisconnect"
                        label={'Vibrate on Bluetooth disconnect'}
                        checked={state.bluetoothDisconnect}
                        onChange={this.onChange.bind(this, 'bluetoothDisconnect')}
                    />
                    <ToggleField
                        fieldName="muteOnQuiet"
                        label={'Mute vibrations on Quiet Mode'}
                        checked={state.muteOnQuiet}
                        onChange={this.onChange.bind(this, 'muteOnQuiet')}
                    />
                    <ToggleField
                        fieldName="updates"
                        label={'Check for updates'}
                        checked={state.update}
                        onChange={this.onChange.bind(this, 'update')}
                    />

                    {this.platform !== 'chalk' &&
                        this.plaform !== 'aplite' && (
                            <div>
                                <ToggleField
                                    fieldName="quickview"
                                    label={'Enable Quickview mode'}
                                    checked={state.quickview}
                                    onChange={this.onChange.bind(this, 'quickview')}
                                />
                                <HelperText>
                                    {
                                        'Hides additional timezone and battery level and adjusts the layout when a timeline event is on the screen.'
                                    }
                                </HelperText>
                            </div>
                        )}
                </OptionGroup>

                <OptionGroup title={'Modules'}>
                    <TabContainer
                        tabs={this.platform === 'chalk' ? this.getEnabledModulesRound() : this.getEnabledModules()}
                    />
                    {this.isEnabled(['15']) &&
                        !this.isEnabledTapWrist(['15']) && (
                            <HelperText standalone={true}>
                                {
                                    '<strong>Alert:</strong> Keeping the compass enabled all the time could drain battery faster. It\'s recommend setting it as a \'Tap\' or \'Shake\' module (enable them below).'
                                }
                            </HelperText>
                        )}
                    {this.platform !== 'aplite' && (
                        <div>
                            <ToggleField
                                fieldName="showSleep"
                                label={'Enable after wake up mode'}
                                checked={state.showSleep}
                                onChange={this.onChange.bind(this, 'showSleep')}
                            />
                            <HelperText>
                                {
                                    'If set, the watchface will show the modules under the \'Sleep\' tab from when you\'re asleep until half an hour after you wake up, switching back to the \'Default\' tab after that. This feature requires Pebble Health enabled.'
                                }
                            </HelperText>
                            <ToggleField
                                fieldName="showTap"
                                label={'Enable tap mode'}
                                checked={state.showTap}
                                onChange={this.onChange.bind(this, 'showTap')}
                            />
                            <ToggleField
                                fieldName="showWrist"
                                label={'Enable wrist shake mode'}
                                checked={state.showWrist}
                                onChange={this.onChange.bind(this, 'showWrist')}
                            />
                            <HelperText>
                                {
                                    '<strong>Experimental features:</strong> If set, the watchface will show the modules under the \'Tap\' tab when you tap the watch screen or the modules under \'Shake\' when you shake your wrist for the amount of time selected below , switching back to the previous view after that. Keep in mind that tap detection is a bit rudimentary because of pebble\'s accelerometer, so light taps might not work. <strong>Enabling these features might drain the battery faster than usual.</strong>'
                                }
                            </HelperText>
                            {(this.state.showTap || this.state.showWrist) && (
                                <RadioButtonGroup
                                    fieldName="tapTime"
                                    label="Tap/Shake mode duration"
                                    options={[
                                        { value: '5', label: '5s' },
                                        { value: '7', label: '7s' },
                                        { value: '10', label: '10s' },
                                    ]}
                                    selectedItem={state.tapTime}
                                    onChange={this.onChange.bind(this, 'tapTime')}
                                />
                            )}
                        </div>
                    )}
                </OptionGroup>

                {this.isEnabled(['18', '19']) && (
                    <OptionGroup title={'Alternate Timezones'}>
                        {this.isEnabled(['18']) && this.renderAlternateTimezone()}

                        {this.isEnabled(['19']) && this.renderAlternateTimezone('B')}
                    </OptionGroup>
                )}

                <OptionGroup title={'Localization'}>
                    <DropdownField
                        fieldName="locale"
                        label={'Language'}
                        options={this.locales}
                        searchable={true}
                        clearable={false}
                        selectedItem={state.locale}
                        onChange={this.onChangeDropdown.bind(this, 'locale')}
                    />
                    <DropdownField
                        fieldName="dateFormat"
                        label={'Date format'}
                        options={this.dateFormatOptions}
                        searchable={false}
                        clearable={false}
                        selectedItem={state.dateFormat}
                        onChange={this.onChangeDropdown.bind(this, 'dateFormat')}
                    />
                    <RadioButtonGroup
                        fieldName="dateSeparator"
                        label="Date separator"
                        options={[
                            { value: '0', label: '(space)' },
                            { value: '1', label: '.' },
                            { value: '2', label: '/' },
                            { value: '3', label: '-' },
                        ]}
                        selectedItem={state.dateSeparator}
                        onChange={this.onChange.bind(this, 'dateSeparator')}
                    />
                </OptionGroup>

                <OptionGroup title={'Colors'}>
                    <ColorPicker
                        fieldName="backgroundColor"
                        label={'Background color'}
                        color={state.bgColor}
                        onChange={this.onChange.bind(this, 'bgColor')}
                    />
                    <ColorPicker
                        fieldName="textColor"
                        label={'Foreground color'}
                        color={state.hoursColor}
                        onChange={this.onChange.bind(this, 'hoursColor')}
                    />
                    <ToggleField
                        fieldName="enableAdvanced"
                        label={'Advanced Colors'}
                        checked={state.enableAdvanced}
                        onChange={this.onChange.bind(this, 'enableAdvanced')}
                    />
                    <HelperText>
                        {
                            'Enables specific color configuration for watchface items. If disabled, all text will have the same color as the selection for \'Foreground Color\' above. This also lets you save and load color presets.'
                        }
                    </HelperText>
                    {state.enableAdvanced && (
                        <div>
                            <ColorPicker
                                fieldName="dateColor"
                                label={'Date color'}
                                color={state.dateColor}
                                onChange={this.onChange.bind(this, 'dateColor')}
                            />
                            {this.isEnabled(['18']) && (
                                <ColorPicker
                                    fieldName="altHoursColor"
                                    label={'Alternate timezone A color'}
                                    color={state.altHoursColor}
                                    onChange={this.onChange.bind(this, 'altHoursColor')}
                                />
                            )}
                            {this.isEnabled(['19']) && (
                                <ColorPicker
                                    fieldName="altHoursBColor"
                                    label={'Alternate timezone B color'}
                                    color={state.altHoursBColor}
                                    onChange={this.onChange.bind(this, 'altHoursBColor')}
                                />
                            )}
                            {this.isEnabled(['17']) && (
                                <ColorPicker
                                    fieldName="batteryColor"
                                    label={'Battery/Low Battery color'}
                                    color={state.batteryColor}
                                    onChange={this.onChange.bind(this, 'batteryColor')}
                                    secondColor={state.batteryLowColor}
                                    onSecondColorChange={this.onChange.bind(this, 'batteryLowColor')}
                                />
                            )}
                            <ColorPicker
                                fieldName="bluetoothColor"
                                label={'Bluetooth disconnected'}
                                color={state.bluetoothColor}
                                onChange={this.onChange.bind(this, 'bluetoothColor')}
                            />
                            <ColorPicker
                                fieldName="updateColor"
                                label={'Update notification'}
                                color={state.updateColor}
                                onChange={this.onChange.bind(this, 'updateColor')}
                            />
                            {this.isEnabled(['1']) && (
                                <ColorPicker
                                    fieldName="weatherColor"
                                    label={'Weather icon/temperature'}
                                    color={state.weatherColor}
                                    onChange={this.onChange.bind(this, 'weatherColor')}
                                    secondColor={state.tempColor}
                                    onSecondColorChange={this.onChange.bind(this, 'tempColor')}
                                />
                            )}
                            {this.isEnabled(['2']) && (
                                <ColorPicker
                                    fieldName="minMaxTemp"
                                    label={'Min/Max temperature'}
                                    color={state.minColor}
                                    onChange={this.onChange.bind(this, 'minColor')}
                                    secondColor={state.maxColor}
                                    onSecondColorChange={this.onChange.bind(this, 'maxColor')}
                                />
                            )}
                            {this.isEnabled(['3']) && (
                                <ColorPicker
                                    fieldName="stepsColor"
                                    label={'Steps/falling behind'}
                                    color={state.stepsColor}
                                    onChange={this.onChange.bind(this, 'stepsColor')}
                                    secondColor={state.stepsBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'stepsBehindColor')}
                                />
                            )}
                            {this.isEnabled(['4']) && (
                                <ColorPicker
                                    fieldName="distColor"
                                    label={'Distance/falling behind'}
                                    color={state.distColor}
                                    onChange={this.onChange.bind(this, 'distColor')}
                                    secondColor={state.distBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'distBehindColor')}
                                />
                            )}
                            {this.isEnabled(['5']) && (
                                <ColorPicker
                                    fieldName="calColor"
                                    label={'Calories/falling behind'}
                                    color={state.calColor}
                                    onChange={this.onChange.bind(this, 'calColor')}
                                    secondColor={state.calBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'calBehindColor')}
                                />
                            )}
                            {this.isEnabled(['6']) && (
                                <ColorPicker
                                    fieldName="sleepColor"
                                    label={'Sleep/falling behind'}
                                    color={state.sleepColor}
                                    onChange={this.onChange.bind(this, 'sleepColor')}
                                    secondColor={state.sleepBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'sleepBehindColor')}
                                />
                            )}
                            {this.isEnabled(['7']) && (
                                <ColorPicker
                                    fieldName="deepColor"
                                    label={'Deep sleep/falling behind'}
                                    color={state.deepColor}
                                    onChange={this.onChange.bind(this, 'deepColor')}
                                    secondColor={state.deepBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'deepBehindColor')}
                                />
                            )}
                            {this.isEnabled(['8']) && (
                                <ColorPicker
                                    fieldName="windSpeedColor"
                                    label={'Wind direction/speed'}
                                    color={state.windDirColor}
                                    onChange={this.onChange.bind(this, 'windDirColor')}
                                    secondColor={state.windSpeedColor}
                                    onSecondColorChange={this.onChange.bind(this, 'windSpeedColor')}
                                />
                            )}
                            {this.isEnabled(['11']) && (
                                <ColorPicker
                                    fieldName="sunriseColor"
                                    label={'Sunrise'}
                                    color={state.sunriseColor}
                                    onChange={this.onChange.bind(this, 'sunriseColor')}
                                />
                            )}
                            {this.isEnabled(['12']) && (
                                <ColorPicker
                                    fieldName="sunsetColor"
                                    label={'Sunset'}
                                    color={state.sunsetColor}
                                    onChange={this.onChange.bind(this, 'sunsetColor')}
                                />
                            )}
                            {this.isEnabled(['13']) && (
                                <ColorPicker
                                    fieldName="activeColor"
                                    label={'Active time/falling behind'}
                                    color={state.activeColor}
                                    onChange={this.onChange.bind(this, 'activeColor')}
                                    secondColor={state.activeBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'activeBehindColor')}
                                />
                            )}
                            {this.isEnabled(['14']) && (
                                <div>
                                    <ColorPicker
                                        fieldName="heartColor"
                                        label={'Heart rate/outside limits'}
                                        color={state.heartColor}
                                        onChange={this.onChange.bind(this, 'heartColor')}
                                        secondColor={state.heartColorOff}
                                        onSecondColorChange={this.onChange.bind(this, 'heartColorOff')}
                                    />
                                    <HelperText>{'Set upper and lower limits in the health section below.'}</HelperText>
                                </div>
                            )}
                            {this.isEnabled(['15']) && (
                                <ColorPicker
                                    fieldName="compassColor"
                                    label={'Compass'}
                                    color={state.compassColor}
                                    onChange={this.onChange.bind(this, 'compassColor')}
                                />
                            )}
                            {this.isEnabled(['16']) && (
                                <ColorPicker
                                    fieldName="secondsColor"
                                    label={'Seconds'}
                                    color={state.secondsColor}
                                    onChange={this.onChange.bind(this, 'secondsColor')}
                                />
                            )}
                            {this.isEnabled(['20']) && (
                                <ColorPicker
                                    fieldName="cryptoColor"
                                    label={'Cryptocurrency price A color'}
                                    color={state.cryptoColor}
                                    onChange={this.onChange.bind(this, 'cryptoColor')}
                                />
                            )}
                            {this.isEnabled(['21']) && (
                                <ColorPicker
                                    fieldName="cryptoBColor"
                                    label={'Cryptocurrency price B color'}
                                    color={state.cryptoBColor}
                                    onChange={this.onChange.bind(this, 'cryptoBColor')}
                                />
                            )}
                            {this.isEnabled(['22']) && (
                                <ColorPicker
                                    fieldName="cryptoCColor"
                                    label={'Cryptocurrency price C color'}
                                    color={state.cryptoCColor}
                                    onChange={this.onChange.bind(this, 'cryptoCColor')}
                                />
                            )}
                            {this.isEnabled(['23']) && (
                                <ColorPicker
                                    fieldName="cryptoDColor"
                                    label={'Cryptocurrency price D color'}
                                    color={state.cryptoDColor}
                                    onChange={this.onChange.bind(this, 'cryptoDColor')}
                                />
                            )}
                        </div>
                    )}
                </OptionGroup>

                {state.enableAdvanced && (
                    <OptionGroup title={'Color Presets'}>
                        <ColorPresets
                            colors={this.getCurrentColors()}
                            onSelect={this.onPresetSelect}
                            storePresets={this.storePresets}
                            presets={state.presets}
                        />
                    </OptionGroup>
                )}

                {this.isWeatherEnabled() && (
                    <OptionGroup title={'Weather'}>
                        <DropdownField
                            fieldName="provider"
                            label={'Weather provider'}
                            options={[
                                { value: '0', label: 'OpenWeatherMap' },
                                { value: '1', label: 'WeatherUnderground' },
                                { value: '2', label: 'Yahoo' },
                                { value: '3', label: 'Dark Sky/Forecast.io' },
                            ]}
                            selectedItem={state.weatherProvider}
                            onChange={this.onChangeDropdown.bind(this, 'weatherProvider')}
                        />

                        {this.isProviderSelected('0') && (
                            <APIKey
                                keyName="openWeatherKey"
                                value={state.openWeatherKey}
                                onChange={this.onChange.bind(this, 'openWeatherKey')}
                                helperText={
                                    '<strong>Note:</strong> Unfortunately, the API key previously used by Timeboxed couldn\'t handle all the users, so from now you need your own API key. Go to <a href="https://home.openweathermap.org/users/sign_up">home.openweathermap.org/users/sign_up</a> to create a free account and get a key and insert it above.'
                                }
                            />
                        )}
                        {this.isProviderSelected('1') && (
                            <APIKey
                                keyName="weatherKey"
                                value={state.weatherKey}
                                onChange={this.onChange.bind(this, 'weatherKey')}
                                helperText={
                                    '<strong>Note:</strong> For WeatherUnderground, you need an API key. Go to <a href="http://www.wunderground.com/weather/api/?apiref=73d2b41a1a02e3bd">wunderground.com</a> to create a free account and get a key and insert it above.'
                                }
                            />
                        )}
                        {this.isProviderSelected('3') && (
                            <APIKey
                                keyName="forecastKey"
                                value={state.forecastKey}
                                onChange={this.onChange.bind(this, 'forecastKey')}
                                helperText={
                                    '<strong>Note:</strong> For Dark Sky/Forecast.io, you need an API key. Go to <a href="https://darksky.net/dev/">darksky.net/dev/</a> to create a free account and get a key and insert it above.'
                                }
                            />
                        )}

                        {this.isEnabled(['1', '2']) && (
                            <ToggleField
                                fieldName="useCelsius"
                                label={'Show temperature in Celsius (ºC)'}
                                checked={state.useCelsius}
                                onChange={this.onChange.bind(this, 'useCelsius')}
                            />
                        )}

                        {this.isEnabled(['1', '2', '8', '11', '12']) && (
                            <RadioButtonGroup
                                fieldName="weatherTime"
                                label={'Refresh interval'}
                                options={refreshTimes}
                                selectedItem={state.weatherTime}
                                onChange={this.onChange.bind(this, 'weatherTime')}
                            />
                        )}

                        {this.isEnabled(['8']) && (
                            <RadioButtonGroup
                                fieldName="speedUnit"
                                label={'Speed unit'}
                                options={[
                                    { value: '0', label: 'mph' },
                                    { value: '1', label: 'km/h' },
                                    { value: '2', label: 'knots' },
                                ]}
                                selectedItem={state.speedUnit}
                                onChange={this.onChange.bind(this, 'speedUnit')}
                            />
                        )}

                        <TextField
                            fieldName="overrideLocation"
                            buttonLabel={'Verify'}
                            label={'Manual Location'}
                            labelPos="top"
                            value={state.overrideLocation}
                            onButtonClick={this.verifyLocation}
                            onChange={this.onChange.bind(this, 'overrideLocation')}
                        />
                        <HelperText>
                            {
                                'If you define a manual location, we won\'t try to use your current location for weather info. (max. length 64 characters). <strong>Note</strong>: If you\'re not sure the city you entered is working, use the \'Verify\' button to check if it\'s a valid location.'
                            }
                        </HelperText>
                    </OptionGroup>
                )}

                {this.isEnabled(['20', '21', '22', '23']) && (
                    <OptionGroup title={'Cryptocurrency prices'}>
                        <HelperText standalone={true}>
                            {
                                'You can use any markets and currencies supported by the <a href="https://www.cryptocompare.com/api/">CryptoCompare API</a>.'
                            }
                        </HelperText>
                        {this.isEnabled(['20']) && this.renderCryptocurrency()}

                        {this.isEnabled(['21']) && this.renderCryptocurrency('B')}

                        {this.isEnabled(['22']) && this.renderCryptocurrency('C')}

                        {this.isEnabled(['23']) && this.renderCryptocurrency('D')}

                        <RadioButtonGroup
                            fieldName="cryptoTime"
                            label={'Refresh interval'}
                            options={refreshTimes}
                            selectedItem={state.cryptoTime}
                            onChange={this.onChange.bind(this, 'cryptoTime')}
                        />
                    </OptionGroup>
                )}

                <OptionGroup title={'Master Key (pmkey.xyz)'}>
                    <TextField
                        fieldName="masterKeyEmail"
                        label={'Email'}
                        value={state.masterKeyEmail}
                        onChange={this.onChange.bind(this, 'masterKeyEmail')}
                    />
                    <TextField
                        fieldName="masterKeyPin"
                        label={'Pin'}
                        value={state.masterKeyPin}
                        onChange={this.onChange.bind(this, 'masterKeyPin')}
                    />
                    <HelperText>
                        {
                            'Enter your email and pin to retrieve your API keys stored with Master Key. We read the WeatherUnderground and/or the DarkSky/Forecast.io API keys.'
                        }
                    </HelperText>
                    <button onClick={this.getMasterKeyData} className="btn btn-primary">
                        {'Retrieve API keys'}
                    </button>
                </OptionGroup>

                {this.isEnabled(['14']) && (
                    <OptionGroup title={'Health'}>
                        <TextField
                            fieldName="heartLow"
                            label={'Lower heart rate limit'}
                            value={state.heartLow}
                            onChange={this.onChange.bind(this, 'heartLow')}
                        />
                        <TextField
                            fieldName="heartHigh"
                            label={'Upper heart rate limit'}
                            value={state.heartHigh}
                            onChange={this.onChange.bind(this, 'heartHigh')}
                        />
                        <HelperText>
                            {
                                'If any of the values are set and different than zero we\'ll show the heart rate in a different color when it\'s below the lower limit or above the upper limit.'
                            }
                        </HelperText>
                    </OptionGroup>
                )}

                <div className="block--footer">
                    <HelperText standalone={true}>{'Remember to save to apply your settings.'}</HelperText>
                    <HelperText standalone={true}>
                        {
                            'Fonts: <a href="http://www.dafont.com/blocko.font">Blocko</a>, <a href="https://fontlibrary.org/en/font/osp-din">OSP-DIN</a>, <a href="https://www.google.com/fonts/specimen/Archivo+Narrow">Archivo Narrow</a> and <a href="http://www.dafont.com/prototype.font">Prototype</a>.<br />Weather font used: <a href="https://erikflowers.github.io/weather-icons/">Erik Flower\'s Weather Icons</a>. Cryptocurrencies API by <a href="https://www.cryptocompare.com/api/">CryptoCompare</a>'
                        }
                    </HelperText>
                    <HelperText standalone={true}>{'If you like Timeboxed, please consider donating ;)'}</HelperText>
                    <DonateButton service="PayPal" />
                    <HelperText standalone={true}>{'US only:'}</HelperText>
                    <div className="donate-buttons">
                        <DonateButton service="SquareCash" />
                        <DonateButton service="Venmo" />
                    </div>
                    <div className="block--submit">
                        <button onClick={this.onSubmit} className="btn btn-primary btn-lg btn-submit">
                            {'Save settings'}
                        </button>
                    </div>
                </div>
            </div>
        );
    }
}

Layout.propTypes = {
    onSubmit: PropTypes.func,
    state: PropTypes.object,
};

Layout.defaultProps = {};

Layout.childContextTypes = {
    locale: PropTypes.string,
};

export default Layout;

/**
 * APIKey component.
 *
 * @@:APIKey
 *
 * @param {[type]} props
 */
const APIKey = (props) => {
    return (
        <div>
            <TextField fieldName={props.keyName} label={'API Key'} value={props.value} onChange={props.onChange} />
            <HelperText>{props.helperText}</HelperText>
        </div>
    );
};

APIKey.propTypes = {
    value: PropTypes.string,
    onChange: PropTypes.func,
    keyName: PropTypes.string,
    helperText: PropTypes.string,
};
APIKey.defaultProps = {};

/**
 * Color picker component.
 *
 * @@:ColorPicker
 *
 * @type {ColorPicker}
 */
class ColorPicker extends Component {
    constructor(props) {
        super(props);
        this.state = {
            color: props.color,
            secondColor: props.secondColor,
            panelVisible: false,
            secondPanelVisible: false,
        };
    }

    onColorChange = (color) => {
        this.setState({ color: color });
        if (this.props.onChange) {
            this.props.onChange(color);
        }
        this.hidePanel();
    };

    onSecondColorChange = (color) => {
        this.setState({ secondColor: color });
        if (this.props.onSecondColorChange) {
            this.props.onSecondColorChange(color);
        }
        this.hidePanel();
    };

    showPanel = () => {
        this.setState({ panelVisible: true });
    };

    hidePanel = () => {
        this.setState({ panelVisible: false, secondPanelVisible: false });
    };

    togglePanel = () => {
        this.setState({ panelVisible: !this.state.panelVisible });
    };

    toggleSecondPanel = () => {
        this.setState({ secondPanelVisible: !this.state.secondPanelVisible });
    };

    shouldComponentUpdate(nextProps, nextState) {
        return this.state !== nextState;
    }

    componentWillReceiveProps(nextProps) {
        if (this.state.color !== nextProps.color || this.state.secondColor !== nextProps.secondColor) {
            this.setState({
                color: nextProps.color,
                secondColor: nextProps.secondColor,
            });
        }
    }

    render() {
        return (
            <div className="color-picker">
                <Field fieldName={this.props.fieldName} label={this.props.label} longLabel={true}>
                    <div className="swatch" onClick={this.togglePanel}>
                        <div className="swatch--color" style={{ backgroundColor: this.state.color }} />
                    </div>
                    {this.state.secondColor ? (
                        <div className="swatch" onClick={this.toggleSecondPanel}>
                            <div className="swatch--color" style={{ backgroundColor: this.state.secondColor }} />
                        </div>
                    ) : null}
                </Field>
                {this.state.panelVisible || this.state.secondPanelVisible ? (
                    <div className="color-panel">
                        <Swatches
                            onColorChange={this.state.panelVisible ? this.onColorChange : this.onSecondColorChange}
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
    label: PropTypes.string,
};

ColorPicker.defaultProps = {};

/**
 * Swatches component.
 *
 * @@:Swatches
 *
 * @type {Swatches}
 */
class Swatches extends Component {
    constructor(props) {
        super(props);

        this.state = {
            sunny: false,
        };

        this.colors = [
            [false, false, '#55ff00', '#aaff55', false, '#ffff55', '#ffffaa', false, false],
            [false, '#aaffaa', '#55ff55', '#00ff00', '#aaff00', '#ffff00', '#ffaa55', '#ffaaaa', false],
            ['#55ffaa', '#00ff55', '#00aa00', '#55aa00', '#aaaa55', '#aaaa00', '#ffaa00', '#ff5500', '#ff5555'],
            ['#aaffff', '#00ffaa', '#00aa55', '#55aa55', '#005500', '#555500', '#aa5500', '#ff0000', '#ff0055'],
            [false, '#55aaaa', '#00aaaa', '#005555', '#ffffff', '#000000', '#aa5555', '#aa0000', false],
            ['#55ffff', '#00ffff', '#00aaff', '#0055aa', '#aaaaaa', '#555555', '#550000', '#aa0055', '#ff55aa'],
            ['#55aaff', '#0055ff', '#0000ff', '#0000aa', '#000055', '#550055', '#aa00aa', '#ff00aa', '#ffaaff'],
            [false, '#5555aa', '#5555ff', '#5500ff', '#5500aa', '#aa00ff', '#ff00ff', '#ff55ff', false],
            [false, false, false, '#aaaaff', '#aa55ff', '#aa55aa', false, false, false],
        ];

        this.sunlightColorMap = {
            '#000000': '#000000',
            '#000055': '#001e41',
            '#0000aa': '#004387',
            '#0000ff': '#0068ca',
            '#005500': '#2b4a2c',
            '#005555': '#27514f',
            '#0055aa': '#16638d',
            '#0055ff': '#007dce',
            '#00aa00': '#5e9860',
            '#00aa55': '#5c9b72',
            '#00aaaa': '#57a5a2',
            '#00aaff': '#4cb4db',
            '#00ff00': '#8ee391',
            '#00ff55': '#8ee69e',
            '#00ffaa': '#8aebc0',
            '#00ffff': '#84f5f1',
            '#550000': '#4a161b',
            '#550055': '#482748',
            '#5500aa': '#40488a',
            '#5500ff': '#2f6bcc',
            '#555500': '#564e36',
            '#555555': '#545454',
            '#5555aa': '#4f6790',
            '#5555ff': '#4180d0',
            '#55aa00': '#759a64',
            '#55aa55': '#759d76',
            '#55aaaa': '#71a6a4',
            '#55aaff': '#69b5dd',
            '#55ff00': '#9ee594',
            '#55ff55': '#9de7a0',
            '#55ffaa': '#9becc2',
            '#55ffff': '#95f6f2',
            '#aa0000': '#99353f',
            '#aa0055': '#983e5a',
            '#aa00aa': '#955694',
            '#aa00ff': '#8f74d2',
            '#aa5500': '#9d5b4d',
            '#aa5555': '#9d6064',
            '#aa55aa': '#9a7099',
            '#aa55ff': '#9587d5',
            '#aaaa00': '#afa072',
            '#aaaa55': '#aea382',
            '#aaaaaa': '#ababab',
            '#ffffff': '#ffffff',
            '#aaaaff': '#a7bae2',
            '#aaff00': '#c9e89d',
            '#aaff55': '#c9eaa7',
            '#aaffaa': '#c7f0c8',
            '#aaffff': '#c3f9f7',
            '#ff0000': '#e35462',
            '#ff0055': '#e25874',
            '#ff00aa': '#e16aa3',
            '#ff00ff': '#de83dc',
            '#ff5500': '#e66e6b',
            '#ff5555': '#e6727c',
            '#ff55aa': '#e37fa7',
            '#ff55ff': '#e194df',
            '#ffaa00': '#f1aa86',
            '#ffaa55': '#f1ad93',
            '#ffaaaa': '#efb5b8',
            '#ffaaff': '#ecc3eb',
            '#ffff00': '#ffeeab',
            '#ffff55': '#fff1b5',
            '#ffffaa': '#fff6d3',
        };

        this.bwColors = [['#000000', '#AAAAAA', '#FFFFFF']];

        this.sunlightColors = this.colors.map((list) => list.map((item) => this.sunlightColorMap[item]));
    }

    onColorTypeChange = (colorType) => {
        this.setState({ sunny: parseInt(colorType, 10) === 1 });
    };

    render() {
        return (
            <div className="color-panel--swatches">
                {getPlatform() === 'aplite' || getPlatform() === 'diorite' ? (
                    <SwatchRows colors={this.bwColors} onColorChange={this.props.onColorChange} />
                ) : (
                    <div>
                        <RadioButtonGroup
                            fieldName="colorType"
                            size="small"
                            options={[{ value: '0', label: 'Normal' }, { value: '1', label: 'Sunny' }]}
                            selectedItem={this.state.sunny ? '1' : '0'}
                            onChange={this.onColorTypeChange}
                        />
                        <FastClick>
                            {' '}
                            {this.state.sunny ? (
                                <SwatchRows colors={this.sunlightColors} onColorChange={this.props.onColorChange} />
                            ) : (
                                <SwatchRows colors={this.colors} onColorChange={this.props.onColorChange} />
                            )}
                        </FastClick>
                    </div>
                )}
            </div>
        );
    }
}

Swatches.propTypes = {
    onColorChange: PropTypes.func,
};

Swatches.defaultProps = {};

/**
 * SwatchRows component.
 *
 * @@:SwatchRows
 *
 * @param {[type]} props
 */
const SwatchRows = (props) => {
    return (
        <div>
            {props.colors.map((list, index) => {
                return (
                    <div className="color-panel--swatch-row" key={index}>
                        {list.map((color, index) => (
                            <SwatchItem
                                color={color}
                                hasBorder={color === '#FFFFFF'}
                                onClick={props.onColorChange}
                                key={index}
                            />
                        ))}
                    </div>
                );
            })}
        </div>
    );
};

SwatchRows.propTypes = {
    colors: PropTypes.arrayOf(PropTypes.string),
    onColorChange: PropTypes.func,
};

/**
 * SwatchItem Component.
 *
 * @@:SwatchItem
 *
 * @type {[type]}
 */
class SwatchItem extends Component {
    onClickHandler = () => {
        if (this.props.color) {
            this.props.onClick(this.props.color);
        }
    };

    render() {
        let style = {};
        if (this.props.color) {
            style.backgroundColor = this.props.color;
        }
        if (this.props.hasBorder) {
            style.border = '1px solid #CCC';
        }
        return <div className="color-panel--swatch-item" onClick={this.onClickHandler} style={style} />;
    }
}

SwatchItem.propTypes = {
    color: PropTypes.string,
    hasBorder: PropTypes.bool,
    onClick: PropTypes.func,
};

/**
 * Color Presets component.
 *
 * @@:ColorPresets
 *
 * @type {ColorPresets}
 */
class ColorPresets extends Component {
    constructor(props) {
        super(props);

        const w = '#FFFFFF';
        const b = '#000000';
        const l = '#AAAAAA';
        const m = '#555555';
        const g = '#00AA00';
        const o = '#FFAA00';
        const y = '#FFFF00';
        const r = '#FF0055';
        const e = '#FF5500';
        const u = '#00AAFF';
        const d = '#0055AA';

        this.defaultPresets = {
            'Black and white': {
                bgColor: b,
                hoursColor: w,
                dateColor: w,
                altHoursColor: w,
                altHoursBColor: w,
                batteryColor: w,
                batteryLowColor: w,
                bluetoothColor: w,
                updateColor: w,
                weatherColor: w,
                tempColor: w,
                minColor: w,
                maxColor: w,
                stepsColor: w,
                stepsBehindColor: w,
                distColor: w,
                distBehindColor: w,
                calColor: w,
                calBehindColor: w,
                sleepColor: w,
                sleepBehindColor: w,
                deepColor: w,
                deepBehindColor: w,
                windDirColor: w,
                windSpeedColor: w,
                sunriseColor: w,
                sunsetColor: w,
                activeColor: w,
                activeBehindColor: w,
                heartColor: w,
                heartColorOff: w,
                compassColor: w,
                secondsColor: w,
                cryptoColor: w,
                cryptoBColor: w,
                cryptoCColor: w,
                cryptoDColor: w,
            },
            Colorful: {
                bgColor: '#000055',
                hoursColor: w,
                dateColor: '#AAFFFF',
                altHoursColor: '#00FFFF',
                altHoursBColor: '#00FFFF',
                batteryColor: l,
                batteryLowColor: e,
                bluetoothColor: e,
                updateColor: '#00FF00',
                weatherColor: y,
                tempColor: y,
                minColor: '#00FFFF',
                maxColor: e,
                stepsColor: '#AAFFFF',
                stepsBehindColor: y,
                distColor: '#AAFFFF',
                distBehindColor: y,
                calColor: '#AAFFFF',
                calBehindColor: y,
                sleepColor: '#AAFFFF',
                sleepBehindColor: y,
                deepColor: '#AAFFFF',
                deepBehindColor: y,
                windDirColor: '#55FF00',
                windSpeedColor: '#55FF00',
                sunriseColor: y,
                sunsetColor: o,
                activeColor: '#AAFFFF',
                activeBehindColor: y,
                heartColor: '#AAFFFF',
                heartColorOff: y,
                compassColor: y,
                secondsColor: l,
                cryptoColor: '#AAFFFF',
                cryptoBColor: '#AAFFFF',
                cryptoCColor: '#AAFFFF',
                cryptoDColor: '#AAFFFF',
            },
            'Green on black': {
                bgColor: b,
                hoursColor: w,
                dateColor: '#00FF00',
                altHoursColor: g,
                altHoursBColor: g,
                batteryColor: l,
                batteryLowColor: e,
                bluetoothColor: e,
                updateColor: '#00FF00',
                weatherColor: '#00FF00',
                tempColor: '#00FF00',
                minColor: l,
                maxColor: g,
                stepsColor: g,
                stepsBehindColor: l,
                distColor: g,
                distBehindColor: l,
                calColor: g,
                calBehindColor: l,
                sleepColor: g,
                sleepBehindColor: l,
                deepColor: g,
                deepBehindColor: l,
                windDirColor: l,
                windSpeedColor: l,
                sunriseColor: g,
                sunsetColor: l,
                activeColor: g,
                activeBehindColor: l,
                heartColor: g,
                heartColorOff: l,
                compassColor: g,
                secondsColor: l,
                cryptoColor: g,
                cryptoBColor: g,
                cryptoCColor: g,
                cryptoDColor: g,
            },
            'Yellow/Orange on black': {
                bgColor: b,
                hoursColor: w,
                dateColor: y,
                altHoursColor: o,
                altHoursBColor: o,
                batteryColor: l,
                batteryLowColor: e,
                bluetoothColor: e,
                updateColor: y,
                weatherColor: y,
                tempColor: y,
                minColor: l,
                maxColor: o,
                stepsColor: o,
                stepsBehindColor: l,
                distColor: o,
                distBehindColor: l,
                calColor: o,
                calBehindColor: l,
                sleepColor: o,
                sleepBehindColor: l,
                deepColor: o,
                deepBehindColor: l,
                windDirColor: l,
                windSpeedColor: l,
                sunriseColor: o,
                sunsetColor: l,
                activeColor: o,
                activeBehindColor: l,
                heartColor: o,
                heartColorOff: l,
                compassColor: o,
                secondsColor: l,
                cryptoColor: o,
                cryptoBColor: o,
                cryptoCColor: o,
                cryptoDColor: o,
            },
            'Blue on black': {
                bgColor: b,
                hoursColor: w,
                dateColor: '#00FFFF',
                altHoursColor: u,
                altHoursBColor: u,
                batteryColor: l,
                batteryLowColor: e,
                bluetoothColor: e,
                updateColor: '#00FFFF',
                weatherColor: '#00FFFF',
                tempColor: '#00FFFF',
                minColor: l,
                maxColor: u,
                stepsColor: u,
                stepsBehindColor: l,
                distColor: u,
                distBehindColor: l,
                calColor: u,
                calBehindColor: l,
                sleepColor: u,
                sleepBehindColor: l,
                deepColor: u,
                deepBehindColor: l,
                windDirColor: l,
                windSpeedColor: l,
                sunriseColor: u,
                sunsetColor: l,
                activeColor: u,
                activeBehindColor: l,
                heartColor: u,
                heartColorOff: l,
                compassColor: u,
                secondsColor: l,
                cryptoColor: u,
                cryptoBColor: u,
                cryptoCColor: u,
                cryptoDColor: u,
            },
            'Red on black': {
                bgColor: b,
                hoursColor: w,
                dateColor: '#FF5555',
                altHoursColor: r,
                altHoursBColor: r,
                batteryColor: l,
                batteryLowColor: o,
                bluetoothColor: o,
                updateColor: '#FF5555',
                weatherColor: '#FF5555',
                tempColor: '#FF5555',
                minColor: l,
                maxColor: r,
                stepsColor: r,
                stepsBehindColor: l,
                distColor: r,
                distBehindColor: l,
                calColor: r,
                calBehindColor: l,
                sleepColor: r,
                sleepBehindColor: l,
                deepColor: r,
                deepBehindColor: l,
                windDirColor: l,
                windSpeedColor: l,
                sunriseColor: r,
                sunsetColor: l,
                activeColor: r,
                activeBehindColor: l,
                heartColor: r,
                heartColorOff: l,
                compassColor: r,
                secondsColor: l,
                cryptoColor: r,
                cryptoBColor: r,
                cryptoCColor: r,
                cryptoDColor: r,
            },
            'Black and white (inverted)': {
                bgColor: w,
                hoursColor: b,
                dateColor: b,
                altHoursColor: b,
                altHoursBColor: b,
                batteryColor: b,
                batteryLowColor: b,
                bluetoothColor: b,
                updateColor: b,
                weatherColor: b,
                tempColor: b,
                minColor: b,
                maxColor: b,
                stepsColor: b,
                stepsBehindColor: b,
                distColor: b,
                distBehindColor: b,
                calColor: b,
                calBehindColor: b,
                sleepColor: b,
                sleepBehindColor: b,
                deepColor: b,
                deepBehindColor: b,
                windDirColor: b,
                windSpeedColor: b,
                sunriseColor: b,
                sunsetColor: b,
                activeColor: b,
                activeBehindColor: b,
                heartColor: b,
                heartColorOff: b,
                compassColor: b,
                secondsColor: b,
                cryptoColor: b,
                cryptoBColor: b,
                cryptoCColor: b,
                cryptoDColor: b,
            },
            'Green on white': {
                bgColor: w,
                hoursColor: b,
                dateColor: '#005500',
                altHoursColor: g,
                altHoursBColor: g,
                batteryColor: m,
                batteryLowColor: '#AA0000',
                bluetoothColor: '#AA0000',
                updateColor: '#005500',
                weatherColor: '#005500',
                tempColor: '#005500',
                minColor: m,
                maxColor: g,
                stepsColor: g,
                stepsBehindColor: m,
                distColor: g,
                distBehindColor: m,
                calColor: g,
                calBehindColor: m,
                sleepColor: g,
                sleepBehindColor: m,
                deepColor: g,
                deepBehindColor: m,
                windDirColor: m,
                windSpeedColor: m,
                sunriseColor: g,
                sunsetColor: m,
                activeColor: g,
                activeBehindColor: m,
                heartColor: g,
                heartColorOff: m,
                compassColor: g,
                secondsColor: m,
                cryptoColor: g,
                cryptoBColor: g,
                cryptoCColor: g,
                cryptoDColor: g,
            },
            'Blue on white': {
                bgColor: w,
                hoursColor: b,
                dateColor: '#005555',
                altHoursColor: d,
                altHoursBColor: d,
                batteryColor: m,
                batteryLowColor: '#AA0000',
                bluetoothColor: '#AA0000',
                updateColor: '#005555',
                weatherColor: '#005555',
                tempColor: '#005555',
                minColor: m,
                maxColor: d,
                stepsColor: d,
                stepsBehindColor: m,
                distColor: d,
                distBehindColor: m,
                calColor: d,
                calBehindColor: m,
                sleepColor: d,
                sleepBehindColor: m,
                deepColor: d,
                deepBehindColor: m,
                windDirColor: m,
                windSpeedColor: m,
                sunriseColor: d,
                sunsetColor: m,
                activeColor: d,
                activeBehindColor: m,
                heartColor: d,
                heartColorOff: m,
                compassColor: d,
                secondsColor: m,
                cryptoColor: d,
                cryptoBColor: d,
                cryptoCColor: d,
                cryptoDColor: d,
            },
        };

        this.state = {
            presets: { ...this.defaultPresets, ...props.presets },
            presetName: '',
        };

        this.storePresets();
    }

    componentWillReceiveProps(nextProps) {
        this.setState({
            presets: { ...this.defaultPresets, ...nextProps.presets },
        });
    }

    storePresets = () => {
        let newPresets = { ...this.state.presets };
        Object.keys(this.defaultPresets).forEach((key) => delete newPresets[key]);
        this.props.storePresets(newPresets);
    };

    onClick = (preset, e) => {
        if (confirm(`Apply preset '${preset}'?`)) {
            if (this.props.onSelect) {
                this.props.onSelect(this.state.presets[preset]);
            }
        }
        e.stopPropagation();
    };

    onAddClick = (name) => {
        if (Object.keys(this.defaultPresets).indexOf(name) !== -1) {
            alert(`You can't replace default '${name}' preset. Choose a different name :)`);
            return;
        }
        if (Object.keys(this.state.presets).length >= Object.keys(this.defaultPresets).length + 10) {
            alert(
                'You can\'t have more than 10 custom presets. Please remove an existing preset before adding a new one.'
            );
            return;
        }
        if (Object.keys(this.state.presets).indexOf(name) !== -1) {
            if (!confirm(`This will replace the '${name}' preset. Continue?`)) {
                return;
            }
        }
        let presets = { ...this.state.presets, ...{ [name]: this.props.colors } };
        this.setState({ presets: presets, presetName: '' });
        window.setTimeout(this.storePresets, 0);
    };

    onRemoveClick = (name, e) => {
        if (confirm(`Remove the preset '${name}'?`)) {
            let presets = { ...this.state.presets };
            delete presets[name];
            this.setState({ presets: presets });
            window.setTimeout(this.storePresets, 0);
        }
        e.stopPropagation();
    };

    onTextChange = (text) => {
        this.setState({
            presetName: text,
        });
    };

    render() {
        return (
            <div>
                <div className="list-group">
                    {Object.keys(this.state.presets)
                        .sort()
                        .map((key) => {
                            return (
                                <li className="list-group-item" key={key} onClick={this.onClick.bind(this, key)}>
                                    {key}
                                    {Object.keys(this.defaultPresets).indexOf(key) === -1 ? (
                                        <span className="remove-preset" onClick={this.onRemoveClick.bind(this, key)}>
                                            &#x2573;
                                        </span>
                                    ) : null}
                                </li>
                            );
                        })}
                </div>
                <TextField
                    fieldName="presetName"
                    buttonLabel={'Add New'}
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
    storePresets: PropTypes.func,
};

ColorPresets.defaultProps = {};

/**
 * DonateButton component.
 *
 * @@:DonateButton
 *
 * @type {[type]}
 */
class DonateButton extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            text: `Donate (via ${this.props.service})`,
        };
    }

    changeText = () => {
        this.setState({
            text: `Redirecting to ${this.props.service}...`,
        });
    };

    renderButton = () => {
        return (
            <input
                type="submit"
                value={this.state.text}
                onClick={this.changeText}
                id="donateBtn"
                className="btn btn--donate"
            />
        );
    };

    render() {
        return (
            <div>
                {this.props.service === 'PayPal' && (
                    <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
                        <input type="hidden" name="cmd" value="_s-xclick" />
                        <input
                            type="hidden"
                            name="encrypted"
                            value="-----BEGIN PKCS7-----MIIHTwYJKoZIhvcNAQcEoIIHQDCCBzwCAQExggEwMIIBLAIBADCBlDCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20CAQAwDQYJKoZIhvcNAQEBBQAEgYC3bkP+CQqxFkzcRG2qGaKBer5KicQj9154qHrm0j7/7dXKycI0i3vBNwwrdage4Dcw07bkGte7luatMIVTNL5F8YnluveT9T5guLR0x1o8tBnnqUH67R/4Fw5MNPt9kxff5ioGFrtkj7TTY72Wgtq6aR92RcxEwxgRVLJhhpEbbzELMAkGBSsOAwIaBQAwgcwGCSqGSIb3DQEHATAUBggqhkiG9w0DBwQI2IrKROAeZKyAgagygOgkzjeOlEvTmAMT4RRiayeR63wIdmlqnftgP2n+6iKc4bMaZ4PxL43rMYRkU5JF3XKaKrRMA1doKlnO09LcQbnm1Y8Uujau/sF/pcF/lzlzd1hjEHVZ7cJ+8FDCsLL79twF5HR2kjuCkGkDen5zt1LloKWkBoNq84A/uq3k765jnJP6DHIirSMvnGCX8+Vk/vg3jBwq4brh1w5plfZHO+roT1V4/bGgggOHMIIDgzCCAuygAwIBAgIBADANBgkqhkiG9w0BAQUFADCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20wHhcNMDQwMjEzMTAxMzE1WhcNMzUwMjEzMTAxMzE1WjCBjjELMAkGA1UEBhMCVVMxCzAJBgNVBAgTAkNBMRYwFAYDVQQHEw1Nb3VudGFpbiBWaWV3MRQwEgYDVQQKEwtQYXlQYWwgSW5jLjETMBEGA1UECxQKbGl2ZV9jZXJ0czERMA8GA1UEAxQIbGl2ZV9hcGkxHDAaBgkqhkiG9w0BCQEWDXJlQHBheXBhbC5jb20wgZ8wDQYJKoZIhvcNAQEBBQADgY0AMIGJAoGBAMFHTt38RMxLXJyO2SmS+Ndl72T7oKJ4u4uw+6awntALWh03PewmIJuzbALScsTS4sZoS1fKciBGoh11gIfHzylvkdNe/hJl66/RGqrj5rFb08sAABNTzDTiqqNpJeBsYs/c2aiGozptX2RlnBktH+SUNpAajW724Nv2Wvhif6sFAgMBAAGjge4wgeswHQYDVR0OBBYEFJaffLvGbxe9WT9S1wob7BDWZJRrMIG7BgNVHSMEgbMwgbCAFJaffLvGbxe9WT9S1wob7BDWZJRroYGUpIGRMIGOMQswCQYDVQQGEwJVUzELMAkGA1UECBMCQ0ExFjAUBgNVBAcTDU1vdW50YWluIFZpZXcxFDASBgNVBAoTC1BheVBhbCBJbmMuMRMwEQYDVQQLFApsaXZlX2NlcnRzMREwDwYDVQQDFAhsaXZlX2FwaTEcMBoGCSqGSIb3DQEJARYNcmVAcGF5cGFsLmNvbYIBADAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBBQUAA4GBAIFfOlaagFrl71+jq6OKidbWFSE+Q4FqROvdgIONth+8kSK//Y/4ihuE4Ymvzn5ceE3S/iBSQQMjyvb+s2TWbQYDwcp129OPIbD9epdr4tJOUNiSojw7BHwYRiPh58S1xGlFgHFXwrEBb3dgNbMUa+u4qectsMAXpVHnD9wIyfmHMYIBmjCCAZYCAQEwgZQwgY4xCzAJBgNVBAYTAlVTMQswCQYDVQQIEwJDQTEWMBQGA1UEBxMNTW91bnRhaW4gVmlldzEUMBIGA1UEChMLUGF5UGFsIEluYy4xEzARBgNVBAsUCmxpdmVfY2VydHMxETAPBgNVBAMUCGxpdmVfYXBpMRwwGgYJKoZIhvcNAQkBFg1yZUBwYXlwYWwuY29tAgEAMAkGBSsOAwIaBQCgXTAYBgkqhkiG9w0BCQMxCwYJKoZIhvcNAQcBMBwGCSqGSIb3DQEJBTEPFw0xNjAzMDgxNjQyNTlaMCMGCSqGSIb3DQEJBDEWBBRGK1SUuKL1oCEAv0MPwjBsklD8/zANBgkqhkiG9w0BAQEFAASBgCmdgq83euPjgcfG4DxFdwj34lOav+IkWl2X+9SDjMRSiFbzYY/Cya+xrFyIsSocN1FkA26hk5VO6+3jvfTI/qg56FNJ7GEqCpStOjH8B9F/SQNGEpq0WHrM/UOJNNS33VioQ1IC2Bm0efWPifQIGxKI5Ku0Q+8HoA7Zz2Rgd7Xl-----END PKCS7-----"
                        />
                        {this.renderButton()}
                        <img
                            alt=""
                            border="0"
                            src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif"
                            width="1"
                            height="1"
                        />
                    </form>
                )}
                {this.props.service === 'SquareCash' && (
                    <form action="https://cash.me/$lbento" method="get" target="_top">
                        {this.renderButton()}
                    </form>
                )}
                {this.props.service === 'Venmo' && (
                    <form
                        action="https://venmo.com/?txn=pay&note=Supporting%20Timeboxed&recipients=lbento"
                        method="get"
                        target="_top">
                        <input type="hidden" name="txn" value="pay" />
                        <input type="hidden" name="note" value="Supporting Timeboxed" />
                        <input type="hidden" name="recipients" value="lbento" />
                        {this.renderButton()}
                    </form>
                )}
            </div>
        );
    }
}

DonateButton.propTypes = {
    service: PropTypes.string.isRequired,
};

/**
 * DropdownField component.
 *
 * @@:DropdownField
 *
 * @type {Object}
 */
class DropdownField extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedItem:
                typeof props.selectedItem !== 'undefined'
                    ? props.selectedItem
                    : props.clearable ? null : props.options[0].value,
        };
    }

    onChange = (value) => {
        if (this.props.onChange) {
            this.props.onChange(value);
        }
        this.setState({ selectedItem: value ? value.value : null });
    };

    componentWillReceiveProps(nextProps) {
        this.setState({ selectedItem: nextProps.selectedItem });
    }

    render() {
        return (
            <Field fieldName={this.props.fieldName} label={this.props.label} labelPos={this.props.labelPos}>
                <FastClick>
                    <Dropdown
                        options={this.props.options}
                        name={this.props.name}
                        value={this.props.options.find((option) => option.value === this.state.selectedItem)}
                        onChange={this.onChange}
                        placeholder={'Select...'}
                        className={'dropdown'}
                    />
                </FastClick>
            </Field>
        );
    }
}

DropdownField.propTypes = {
    selectedItem: PropTypes.string,
    clearable: PropTypes.bool,
    options: PropTypes.arrayOf(PropTypes.object),
    onChange: PropTypes.func,
    name: PropTypes.string,
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPos: PropTypes.string,
    searchable: PropTypes.bool,
};

DropdownField.defaultProps = {
    searchable: false,
    clearable: false,
};

/**
 * Field Component
 *
 * @@:Field
 *
 * @type {[type]}
 */
class Field extends React.Component {
    render() {
        let classes = classnames({
            [`field field--${this.props.fieldName}`]: true,
            'field--long-label': this.props.longLabel,
            'field--long': this.props.labelPos === 'top',
            'field--long-reverse': this.props.labelPos === 'bottom',
        });
        return (
            <div className="field-container">
                <div className={classes}>
                    {this.props.label ? <label className="field-label">{this.props.label}</label> : null}
                    <div className="field-content">
                        <div className="field-content-inner">{this.props.children}</div>
                    </div>
                </div>
            </div>
        );
    }
}

Field.propTypes = {
    fieldName: PropTypes.string.isRequired,
    label: PropTypes.string,
    longLabel: PropTypes.bool,
    labelPos: PropTypes.string,
    children: PropTypes.any,
};

/**
 * HelperText component
 *
 * @@:HelperText
 *
 * @param {[type]} props
 */
const HelperText = (props) => {
    let classes = classnames({
        'helper-text': true,
        'helper-text--field': !props.standalone,
        'helper-text--standalone': props.standalone,
    });
    return <div className={classes} dangerouslySetInnerHTML={{ __html: props.children }} />;
};

HelperText.propTypes = {
    standalone: PropTypes.bool,
    children: PropTypes.any,
};

HelperText.defaultProps = {};

/**
 * OptionGroup component
 *
 * @@:OptionGroup
 *
 * @param {[type]} props
 */
const OptionGroup = (props) => {
    return (
        <div className="card card--custom">
            {props.title ? <h3 className="card-header">{props.title}</h3> : null}
            <div className="card-block card-block--custom">{props.children}</div>
        </div>
    );
};

OptionGroup.propTypes = {
    title: PropTypes.string,
    children: PropTypes.any,
};

OptionGroup.defaultProps = {};

/**
 * SideBySideFields component
 *
 * @@:SideBySideFields
 *
 * @param {[type]} props
 */
const SideBySideFields = (props) => {
    return (
        <div className="side-by-side">
            <div className="side-by-side--left">{props.children[0]}</div>
            <div className={classnames({ 'side-by-side--right': true, 'label--right': props.align })}>
                {props.children[1]}
            </div>
        </div>
    );
};

SideBySideFields.propTypes = {
    children: PropTypes.any,
    align: PropTypes.bool,
};

SideBySideFields.defaultProps = {};

/**
 * TabContainer Component
 *
 * @@:TabContainer
 *
 * @type {Object}
 */
class TabContainer extends Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedTab: 0,
        };
    }

    onClick = (selected) => {
        this.setState({ selectedTab: selected });
        if (this.props.onChange) {
            this.props.onChange(selected);
        }
    };

    componentWillReceiveProps(nextProps) {
        if (Object.keys(nextProps.tabs).length - 1 < this.state.selectedTab) {
            this.setState({ selectedTab: 0 });
        }
    }

    render() {
        let titles = Object.keys(this.props.tabs).map((title, index) => {
            let titleClasses = classnames({
                'nav-link': true,
                active: this.state.selectedTab === index,
            });
            return (
                <li className="nav-item" key={title} onClick={this.onClick.bind(this, index)}>
                    <a className={titleClasses}>{title}</a>
                </li>
            );
        });

        let items = Object.keys(this.props.tabs).map((title) => this.props.tabs[title]);

        return (
            <div className="tab-container">
                <ul className="nav nav-tabs">
                    <FastClick>{titles}</FastClick>
                </ul>
                <div className="tab-content">{items[this.state.selectedTab]}</div>
            </div>
        );
    }
}

TabContainer.propTypes = {
    onChange: PropTypes.func,
    tabs: PropTypes.object,
};

TabContainer.defaultProps = {};

/**
 * TextField Component
 *
 * @@:TextField
 *
 * @type {[type]}
 */
class TextField extends Component {
    onChange = (e) => {
        let value = e.target.value;
        if (this.props.onChange) {
            this.props.onChange(value);
        }
    };

    onButtonClick = () => {
        if (this.props.onButtonClick) {
            this.props.onButtonClick(this.props.value);
        }
    };

    render() {
        return (
            <Field
                fieldName={this.props.fieldName}
                label={this.props.label}
                labelPos={this.props.labelPos}
                helperText={this.props.helperText}>
                <div className="field-text">
                    <input
                        type="text"
                        className="form-control"
                        name={this.props.fieldName}
                        value={this.props.value}
                        onChange={this.onChange}
                        placeholder={this.props.placeholder}
                    />
                    {this.props.buttonLabel ? (
                        <button className="btn btn-primary field-text--btn" onClick={this.onButtonClick}>
                            {this.props.buttonLabel}
                        </button>
                    ) : null}
                </div>
            </Field>
        );
    }
}

TextField.propTypes = {
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPos: PropTypes.string,
    helperText: PropTypes.string,
    value: PropTypes.string,
    buttonLabel: PropTypes.string,
    onChange: PropTypes.func,
    onButtonClick: PropTypes.func,
    placeholder: PropTypes.string,
};

TextField.defaultProps = {};

/**
 * ToggleField Component
 *
 * @@:ToggleField
 *
 * @type {Object}
 */
class ToggleField extends React.Component {
    constructor(props) {
        super(props);
        this.state = { checked: !!props.checked };
    }

    componentWillReceiveProps(nextProps) {
        if (this.props.checked !== nextProps.checked) {
            this.toggleCheckbox({ target: { checked: !!nextProps.checked } });
        }
    }

    toggleCheckbox = (e) => {
        let value = !!e.target.checked;
        this.setState({ checked: value });
        if (this.props.onChange) {
            this.props.onChange(value);
        }
    };

    render() {
        return (
            <Field label={this.props.label} fieldName={this.props.fieldName} longLabel={true}>
                <label>
                    <FastClick>
                        <Toggle className="toggle" onChange={this.toggleCheckbox} defaultChecked={this.state.checked} />
                    </FastClick>
                </label>
            </Field>
        );
    }
}

ToggleField.propTypes = {
    checked: PropTypes.bool,
    fieldName: PropTypes.string.isRequired,
    label: PropTypes.string,
    onChange: PropTypes.func,
};

ToggleField.defaultProps = {
    checked: false,
};

/**
 * VersionIndicator Component
 *
 * @@:VersionIndicator
 *
 * @type {Object}
 */
class VersionIndicator extends React.Component {
    constructor(props) {
        super(props);

        this.state = {
            version: getCurrentVersion(),
        };
    }

    onClick = () => {
        if (this.props.onClick) {
            this.props.onClick();
        }
    };

    render() {
        return (
            <span className="version" onClick={this.onClick}>
                {this.state.version ? `v${this.state.version}` : ' '}
            </span>
        );
    }
}

VersionIndicator.propTypes = {
    onClick: PropTypes.func,
};

VersionIndicator.defaultProps = {};

/**
 * RadioButtonGroup Component
 *
 * @@:RadioButtonGroup
 *
 * @type {Object}
 */
class RadioButtonGroup extends React.Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedItem: typeof props.selectedItem !== 'undefined' ? props.selectedItem : props.options['0'].value,
        };
    }

    toggleButton = (index) => {
        this.setState({ selectedItem: index });
        if (this.props.onChange) {
            this.props.onChange(index);
        }
    };

    render() {
        return (
            <Field fieldName={this.props.fieldName} label={this.props.label} labelPos={this.props.labelPos}>
                <FastClick>
                    <div className="btn-group">
                        {this.props.options.map((item) => {
                            let classes = {
                                'btn btn-outline-primary': true,
                                active: this.state.selectedItem === item.value,
                            };
                            if (this.props.size) {
                                classes[this.props.size] = true;
                            }
                            return (
                                <button
                                    key={item.value}
                                    className={classnames(classes)}
                                    onClick={this.toggleButton.bind(this, item.value)}>
                                    {item.label}
                                </button>
                            );
                        })}
                    </div>
                </FastClick>
            </Field>
        );
    }
}

RadioButtonGroup.propTypes = {
    selectedItem: PropTypes.string,
    options: PropTypes.arrayOf(PropTypes.object),
    onChange: PropTypes.func,
    name: PropTypes.string,
    fieldName: PropTypes.string,
    label: PropTypes.string,
    labelPos: PropTypes.string,
    size: PropTypes.string,
};

RadioButtonGroup.defaultProps = {
    labelPos: 'top',
};

// ------------------ UTILS

const getCurrentVersion = () => {
    return window.timeboxedVersion || getQueryParam('v');
};

const getPlatform = () => {
    return window.pebblePlatform || getQueryParam('p');
};

const getConfigs = () => {
    let config = decodeURIComponent(window.timeboxedConfigs || getQueryParam('c'));
    try {
        JSON.parse(decodeURIComponent(config));
        return decodeURIComponent(config);
    } catch (error) {
        return LZString.decompressFromBase64(config);
    }
};

const getReturnUrl = () => {
    return decodeURIComponent(window.pebbleReturnTo || 'pebblejs://close#');
};

const getQueryParam = (variable, defaultValue) => {
    let query = location.search.substring(1);
    let vars = query.split('&');
    for (let i = 0; i < vars.length; i++) {
        let pair = vars[i].split('=');
        if (pair[0] === variable) {
            return decodeURIComponent(pair[1]);
        }
    }
    return defaultValue || '';
};

const providerUrls = {
    '0': 'http://api.openweathermap.org/data/2.5/weather?appid=979cbf006bf67bc368a54af240d15cf3&q=${location}',
    '1': 'http://api.wunderground.com/api/${apiKey}/conditions/forecast/q/${location}.json',
    '2':
        'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=select%20*%20from%20weather.forecast%20where%20woeid%20in%20(select%20woeid%20from%20geo.places%20where%20text%3D%22${location}%22)',
    '3':
        'https://query.yahooapis.com/v1/public/yql?format=json&env=store%3A%2F%2Fdatatables.org%2Falltableswithkeys&q=select%20centroid%20from%20geo.places(1)%20where%20text%3D%22${location}%22',
};

const verifyLocation = (loc, provider, apiKey, callback = () => {}) => {
    if (!loc) {
        callback(false);
        return;
    }
    apiKey = apiKey || '';
    loc = encodeURIComponent(loc);
    let url = providerUrls[provider].replace('${location}', loc).replace('${apiKey}', apiKey);

    fetch(url)
        .then((response) => {
            if (response.status >= 400) {
                throw new Error('Bad response from server');
            }
            return response.json();
        })
        .then((json) => {
            switch (provider) {
                case '0':
                    if (parseInt(json.cod, 10) === 404) {
                        alert('Invalid location');
                        callback(false);
                    } else {
                        alert('Valid location!');
                        callback(true);
                    }
                    break;
                case '1':
                    if (json.response.error || !json.current_observation) {
                        if (json.response.error && json.response.error.type === 'keynotfound') {
                            alert('Invalid WeatherUnderground Key');
                            callback(false);
                        } else {
                            alert('Invalid location');
                            callback(false);
                        }
                    } else {
                        alert('Valid location!');
                        callback(true);
                    }
                    break;
                case '2':
                    if (json.query.count === 0) {
                        alert('Invalid location');
                        callback(false);
                    } else {
                        alert('Valid location!');
                        callback(true);
                    }
                    break;
                case '3':
                    if (json.query.count === 0) {
                        alert('Invalid location');
                        callback(false);
                    } else {
                        alert('Valid location!');
                        callback(true);
                    }
                    break;
                default:
                    callback(false);
                    break;
            }
            return null;
        })
        .catch((ex) => {
            console.log(ex.stack);
            callback(false);
        });
};

const fetchMasterKeyData = (email, pin, callback = () => {}) => {
    if (!email || !pin) {
        callback({});
        return;
    }

    let url = `https://pmkey.xyz/search/?email=${email}&pin=${pin}`;
    fetch(url)
        .then((response) => {
            if (response.status >= 400) {
                throw new Error('Bad response from server');
            }
            return response.json();
        })
        .then((json) => {
            if (!json.success || !json.keys || !json.keys.weather) {
                callback({});
                throw new Error('No keys found');
            } else {
                callback(json.keys.weather);
            }
            return null;
        })
        .catch((ex) => {
            console.log(ex.stack); // eslint-disable-line no-console
            alert(ex.message);
            callback({});
        });
};

//------------------- CONSTANTS

const allTimezones = [
    { value: '#|0:00', label: 'None' },
    { value: 'aoe|-12:00', label: '(GMT -12) Int. Date Line' },
    { value: 'sst|-11:00', label: '(GMT -11) SST: Samoa' },
    { value: 'hast|-10:00', label: '(GMT -10) HAST: Hawaii' },
    { value: 'hadt|-9:00', label: '(GMT -9) HADT: Hawaii (Daylight)' },
    { value: 'akst|-9:00', label: '(GMT -9) AKST: Alaska' },
    { value: 'akdt|-8:00', label: '(GMT -8) AKDT: Alaska (Daylight)' },
    { value: 'pst|-8:00', label: '(GMT -8) PST: Pacific Standard Time' },
    { value: 'pdt|-7:00', label: '(GMT -7) PDT: Pacific Daylight Time' },
    { value: 'mst|-7:00', label: '(GMT -7) MST: Mountain Standard Time' },
    { value: 'mdt|-6:00', label: '(GMT -6) MDT: Mountain Daylight Time' },
    { value: 'cst|-6:00', label: '(GMT -6) CST: Central Standard Time' },
    { value: 'cdt|-5:00', label: '(GMT -5) CDT: Central Daylight Time' },
    { value: 'est|-5:00', label: '(GMT -5) EST: Eastern Standard Time' },
    { value: 'pet|-5:00', label: '(GMT -5) PET: Peru' },
    { value: 'act|-5:00', label: '(GMT -5) ACT: Acre, Brazil' },
    { value: 'vet|-4:30', label: '(GMT -4:30) VET: Venezuela' },
    { value: 'edt|-4:00', label: '(GMT -4) EDT: Eastern Daylight Time' },
    { value: 'ast|-4:00', label: '(GMT -4) AST: Atlantic Standard Time' },
    { value: 'nst|-3:30', label: '(GMT -3:30) NST: Newfoundland Time' },
    { value: 'adt|-3:00', label: '(GMT -3) ADT: Atlantic Daylight Time' },
    { value: 'art|-3:00', label: '(GMT -3) ART: Argentina' },
    { value: 'brt|-3:00', label: '(GMT -3) BRT: Brazil' },
    { value: 'wgt|-3:00', label: '(GMT -3) WGT: West Greenland' },
    {
        value: 'ndt|-2:30',
        label: '(GMT -2:30) NST: Newfoundland Daylight Time',
    },
    {
        value: 'wgst|-2:00',
        label: '(GMT -2) WGST: West Greenland Summer Time',
    },
    { value: 'brst|-2:00', label: '(GMT -2) BRST: Brazil Summer Time' },
    { value: 'egt|-1:00', label: '(GMT -1) EGT: East Greenland' },
    { value: 'azot|-1:00', label: '(GMT -1) AZOT: Azores' },
    { value: 'azost|0:00', label: '(GMT +0) AZOST: Azores Summer' },
    { value: 'egst|0:00', label: '(GMT +0) EGST: East Greenland Summer' },
    { value: 'gmt|0:00', label: 'GMT: Greenwich Mean Time' },
    { value: 'wet|0:00', label: '(GMT +0) WET: Western European Time' },
    { value: 'bst|1:00', label: '(GMT +1) BST: British Summer Time' },
    { value: 'cet|1:00', label: '(GMT +1) CET: Central European Time' },
    { value: 'ist|1:00', label: '(GMT +1) IST: Irish Standard Time' },
    {
        value: 'cest|2:00',
        label: '(GMT +2) CEST: Central European Summer Time',
    },
    { value: 'cat|2:00', label: '(GMT +2) CAT: Central Africa Time' },
    { value: 'eet|2:00', label: '(GMT +2) EET: Eastern European Time' },
    {
        value: 'sast|2:00',
        label: '(GMT +2) SAST: South Africa Standard Time',
    },
    { value: 'eat|3:00', label: '(GMT +3) EAT: East Africa Time' },
    {
        value: 'eest|3:00',
        label: '(GMT +3) EEST: Eastern European Summer Time',
    },
    { value: 'msk|3:00', label: '(GMT +3) MSK: Moscow Standard Time' },
    { value: 'irst|3:30', label: '(GMT +3:30) IRST: Iran Standard Time' },
    { value: 'gst|4:00', label: '(GMT +4) GST: Gulf Standard Time' },
    { value: 'mdk|4:00', label: '(GMT +4) MDK: Moscow Daylight Time' },
    { value: 'irdt|4:30', label: '(GMT +4:30) IRDT: Iran Daylight Time' },
    { value: 'mvt|5:00', label: '(GMT +5) MVT: Maldives Time' },
    { value: 'ist|5:30', label: '(GMT +5:30) IST: India Standard Time' },
    { value: 'bst|6:00', label: '(GMT +6) BST: Bangladesh Standard Time' },
    { value: 'mmt|6:30', label: '(GMT +6:30) MMT: Myanmar Time' },
    { value: 'wib|7:00', label: '(GMT +7) WIB: Western Indonesian Time' },
    { value: 'wita|8:00', label: '(GMT +8) WITA: Central Indonesian Time' },
    {
        value: 'awst|8:00',
        label: '(GMT +8) AWST: Australia Western Standard Time',
    },
    { value: 'cst|8:00', label: '(GMT +8) CST: China Standard Time' },
    { value: 'hkt|8:00', label: '(GMT +8) HKT: Hong Kong Time' },
    { value: 'pyt|8:30', label: '(GMT +8:30) PYT: Pyongyang Time' },
    { value: 'wit|9:00', label: '(GMT +9) WIT: Eastern Indonesian Time' },
    {
        value: 'awdt|9:00',
        label: '(GMT +9) AWDT: Australia Western Daylight Time',
    },
    { value: 'jst|9:00', label: '(GMT +9) JST: Japan Standard Time' },
    { value: 'kst|9:00', label: '(GMT +9) KST: Korea Standard Time' },
    {
        value: 'acst|9:30',
        label: '(GMT +9:30) ACST: Australia Central Standard Time',
    },
    {
        value: 'aest|10:00',
        label: '(GMT +10) AEST: Australia Eastern Standard Time',
    },
    { value: 'pgt|10:00', label: '(GMT +10) PGT: Papua New Guinea Time' },
    {
        value: 'acdt|10:30',
        label: '(GMT +10:30) ACDT: Australia Central Daylight Time',
    },
    {
        value: 'aedt|11:00',
        label: '(GMT +11) AEDT: Australia Eastern Daylight Time',
    },
    { value: 'fjt|12:00', label: '(GMT +12) FJT: Fiji Time' },
    {
        value: 'nzst|12:00',
        label: '(GMT +12) NZST: New Zealand Standard Time',
    },
    { value: 'fjst|13:00', label: '(GMT +13) FJST: Fiji Summer Time' },
    {
        value: 'nzdt|13:00',
        label: '(GMT +13) NZDT: New Zealand Daylight Time',
    },
    { value: 'wst|14:00', label: '(GMT +14) WST: Western Samoa Time' },
];

const allLocales = [
    { value: '0', label: 'English' },
    { value: '1', label: 'Portuguese' },
    { value: '2', label: 'French' },
    { value: '3', label: 'German' },
    { value: '4', label: 'Spanish' },
    { value: '5', label: 'Italian' },
    { value: '6', label: 'Dutch' },
    { value: '7', label: 'Danish' },
    { value: '8', label: 'Turkish' },
    { value: '9', label: 'Czech' },
    { value: '10', label: 'Polish' },
    { value: '11', label: 'Swedish' },
    { value: '12', label: 'Finnish' },
    { value: '13', label: 'Slovak' },
];

const allFonts = [
    { value: '0', label: 'Blocko' },
    { value: '1', label: 'Bloco (big)' },
    { value: '3', label: 'Archivo' },
    { value: '4', label: 'Din' },
    { value: '5', label: 'Prototype' },
    { value: '6', label: 'LECO' },
    { value: '7', label: 'Konstruct' },
];

const allDateOptions = [
    { value: '0', label: 'Day of week, month, day' },
    { value: '1', label: 'Day of week, day, month' },
    { value: '2', label: 'Day of week, day' },
    { value: '3', label: 'Day, month' },
    { value: '4', label: 'Month, day' },
    { value: '5', label: 'Day, month (number)' },
    { value: '6', label: 'Month (number), day' },
    { value: '7', label: 'Day of week, day, month (number)' },
    { value: '8', label: 'Day of week, month (number), day' },
    { value: '9', label: 'ISO-8601 (year, month, day)' },
    { value: '10', label: 'Week number, day, month (number)' },
    { value: '11', label: 'Week number, month (number), day' },
];

const baseModules = [
    [
        {
            name: 't-l',
            label: 'Top Left',
            slot: 'slotA',
            textOnly: false,
            labelPos: 'top',
        },
        {
            name: 't-r',
            label: 'Top Right',
            slot: 'slotB',
            textOnly: false,
            labelPos: 'top',
        },
    ],
    {
        name: 'c-t',
        label: 'Center Top',
        slot: 'slotE',
        textOnly: true,
        labelPos: 'top',
    },
    {
        name: 'center-bottom',
        label: 'Center Bottom',
        slot: 'slotF',
        textOnly: true,
        labelPos: 'bottom',
    },
    [
        {
            name: 'b-l',
            label: 'Bottom Left',
            slot: 'slotC',
            textOnly: false,
            labelPos: 'bottom',
        },
        {
            name: 'b-r',
            label: 'Bottom Right',
            slot: 'slotD',
            textOnly: false,
            labelPos: 'bottom',
        },
    ],
];

const baseModulesRound = [
    {
        name: 't-l',
        label: 'Top 1',
        slot: 'slotA',
        textOnly: false,
        labelPos: 'top',
    },
    {
        name: 't-r',
        label: 'Top 2',
        slot: 'slotB',
        textOnly: false,
        labelPos: 'top',
    },
    {
        name: 'c-t',
        label: 'Center top',
        slot: 'slotE',
        textOnly: true,
        labelPos: 'top',
    },
    {
        name: 'center-bottom',
        label: 'Center bottom',
        slot: 'slotF',
        textOnly: true,
        labelPos: 'bottom',
    },
    {
        name: 'b-l',
        label: 'Bottom 1',
        slot: 'slotC',
        textOnly: false,
        labelPos: 'bottom',
    },
    {
        name: 'b-r',
        label: 'Bottom 2',
        slot: 'slotD',
        textOnly: false,
        labelPos: 'bottom',
    },
];

const defaultState = {
    leadingZero: true,
    bluetoothDisconnect: true,
    update: true,
    timezones: '#|0:00',
    timezonesB: '#|0:00',
    slotA: '1',
    slotB: '2',
    slotC: '3',
    slotD: '4',
    sleepSlotA: '1',
    sleepSlotB: '2',
    sleepSlotC: '6',
    sleepSlotD: '7',
    locale: '0',
    dateFormat: '0',
    textAlign: '2',
    fontType: '6',
    enableAdvanced: false,
    weatherProvider: '2',
    useCelsius: false,
    overrideLocation: '',
    showSleep: false,
    weatherKey: '',
    forecastKey: '',
    speedUnit: '0',
    showDebug: false,
    quickview: true,
    dateSeparator: '1',
    muteOnQuiet: false,
    showTap: false,
    tapTime: '7',
    tapSlotA: '15',
    tapSlotB: '14',
    tapSlotC: '5',
    tapSlotD: '13',
    weatherTime: '30',
    heartLow: '0',
    heartHigh: '0',
    showWrist: false,
    wristSlotA: '11',
    wristSlotB: '12',
    wristSlotC: '8',
    wristSlotD: '16',
    masterKeyEmail: '',
    masterKeyPin: '',
    slotE: '18',
    slotF: '17',
    sleepSlotE: '18',
    sleepSlotF: '17',
    tapSlotE: '18',
    tapSlotF: '17',
    wristSlotE: '18',
    wristSlotF: '17',
    openWeatherKey: '',
    presets: {},
    cryptoFrom: '',
    cryptoTo: '',
    cryptoFromB: '',
    cryptoToB: '',
    cryptoFromC: '',
    cryptoToC: '',
    cryptoFromD: '',
    cryptoToD: '',
    cryptoMarket: 'Coinbase',
    cryptoMarketB: 'Coinbase',
    cryptoMarketC: 'Coinbase',
    cryptoMarketD: 'Coinbase',
    cryptoTime: '15',
    cryptoMulti: false,
    cryptoMultiB: false,
    cryptoMultiC: false,
    cryptoMultiD: false,
};

const cryptoMarkets = [
    { value: 'BTC38', label: 'BTC38' },
    { value: 'BTCC', label: 'BTCC' },
    { value: 'BTCE', label: 'BTCE' },
    { value: 'BTCMarkets', label: 'BTCMarkets' },
    { value: 'BTCXIndia', label: 'BTCXIndia' },
    { value: 'BTER', label: 'BTER' },
    { value: 'Binance', label: 'Binance' },
    { value: 'Bit2C', label: 'Bit2C' },
    { value: 'BitBay', label: 'BitBay' },
    { value: 'BitMarket', label: 'BitMarket' },
    { value: 'BitSquare', label: 'BitSquare' },
    { value: 'Bitfinex', label: 'Bitfinex' },
    { value: 'Bitso', label: 'Bitso' },
    { value: 'Bitstamp', label: 'Bitstamp' },
    { value: 'Bittrex', label: 'Bittrex' },
    { value: 'Bleutrade', label: 'Bleutrade' },
    { value: 'CCEDK', label: 'CCEDK' },
    { value: 'Cexio', label: 'Cexio' },
    { value: 'CoinCheck', label: 'CoinCheck' },
    { value: 'Coinbase', label: 'Coinbase' },
    { value: 'Coinfloor', label: 'Coinfloor' },
    { value: 'Coinone', label: 'Coinone' },
    { value: 'Coinse', label: 'Coinse' },
    { value: 'Coinsetter', label: 'Coinsetter' },
    { value: 'Cryptopia', label: 'Cryptopia' },
    { value: 'Cryptsy', label: 'Cryptsy' },
    { value: 'EtherDelta', label: 'EtherDelta' },
    { value: 'EthexIndia', label: 'EthexIndia' },
    { value: 'Gatecoin', label: 'Gatecoin' },
    { value: 'Gemini', label: 'Gemini' },
    { value: 'HitBTC', label: 'HitBTC' },
    { value: 'Huobi', label: 'Huobi' },
    { value: 'Korbit', label: 'Korbit' },
    { value: 'Kraken', label: 'Kraken' },
    { value: 'LakeBTC', label: 'LakeBTC' },
    { value: 'Liqui', label: 'Liqui' },
    { value: 'LiveCoin', label: 'LiveCoin' },
    { value: 'LocalBitcoins', label: 'LocalBitcoins' },
    { value: 'Luno', label: 'Luno' },
    { value: 'MercadoBitcoin', label: 'MercadoBitcoin' },
    { value: 'MonetaGo', label: 'MonetaGo' },
    { value: 'OKCoin', label: 'OKCoin' },
    { value: 'Paymium', label: 'Paymium' },
    { value: 'Poloniex', label: 'Poloniex' },
    { value: 'QuadrigaCX', label: 'QuadrigaCX' },
    { value: 'Quoine', label: 'Quoine' },
    { value: 'TheRockTrading', label: 'TheRockTrading' },
    { value: 'Tidex', label: 'Tidex' },
    { value: 'Unocoin', label: 'Unocoin' },
    { value: 'Vaultoro', label: 'Vaultoro' },
    { value: 'Yacuna', label: 'Yacuna' },
    { value: 'Yobit', label: 'Yobit' },
    { value: 'Yunbi', label: 'Yunbi' },
    { value: 'bitFlyer', label: 'bitFlyer' },
    { value: 'bitFlyerFX', label: 'bitFlyerFX' },
    { value: 'itBit', label: 'itBit' },
];

const refreshTimes = [
    { value: '5', label: '5min' },
    { value: '10', label: '10min' },
    { value: '15', label: '15min' },
    { value: '30', label: '30min' },
];

const w = '#FFFFFF';

const defaultColors = {
    bgColor: '#000000',
    hoursColor: w,
    dateColor: w,
    altHoursColor: w,
    altHoursBColor: w,
    batteryColor: w,
    batteryLowColor: w,
    bluetoothColor: w,
    updateColor: w,
    weatherColor: w,
    tempColor: w,
    minColor: w,
    maxColor: w,
    stepsColor: w,
    stepsBehindColor: w,
    distColor: w,
    distBehindColor: w,
    calColor: w,
    calBehindColor: w,
    sleepColor: w,
    sleepBehindColor: w,
    deepColor: w,
    deepBehindColor: w,
    windDirColor: w,
    windSpeedColor: w,
    sunriseColor: w,
    sunsetColor: w,
    activeColor: w,
    activeBehindColor: w,
    heartColor: w,
    heartColorOff: w,
    compassColor: w,
    secondsColor: w,
    cryptoColor: w,
    cryptoBColor: w,
    cryptoCColor: w,
    cryptoDColor: w,
};

//----------------- RENDER STUFF

const onSubmit = (data) => {
    document.location = getReturnUrl() + LZString.compressToBase64(JSON.stringify(formatDataToSend(data)));
};

const getStoredData = (source) => {
    return Object.keys(source).reduce((data, key) => {
        let value = source[key] === undefined ? '' : source[key];

        value = value === 'true' || value === 'false' ? JSON.parse(value) : value;
        value = typeof value === 'string' && value.indexOf('0x') !== -1 ? value.replace('0x', '#') : value;

        return { ...data, [key]: value };
    }, {});
};

const getStoredDataFromParams = () => {
    let config = getConfigs();
    config = config ? JSON.parse(config) : {};
    return getStoredData(config);
};
const formatDataToSend = (data) => {
    let newData = Object.keys(data).reduce((items, key) => {
        items[key] = data[key];

        if (key.indexOf('Color') !== -1) {
            items[key] = items[key].replace('#', '0x');
        }

        return items;
    }, {});

    return newData;
};

ReactDOM.render(<Layout onSubmit={onSubmit} state={getStoredDataFromParams()} />, document.getElementById('content'));
