/*eslint-disable no-console*/
import { allTimezones, defaultColors, defaultState } from './util/constants';
import { fetchMasterKeyData, getCurrentVersion, getPlatform, verifyLocation } from './util/util';
import { getLocaleById, getText } from './lang';
import APIKey from './APIKey';
import ColorPicker from './color-picker';
import ColorPresets from './color-presets';
import DonateButton from './donate';
import DropdownField from './dropdown';
import HelperText from './helper-text';
import OptionGroup from './option-group';
import RadioButtonGroup from './button-group';
import React, { Component, PropTypes } from 'react';
import SideBySideFields from './side-by-side-fields';
import TabContainer from './tabs';
import TextField from './text-field';
import ToggleField from './toggle';
import VersionIndicator from './version-indicator';

/* eslint-disable-next-line */
import 'react-select/scss/default.scss';
/* eslint-disable-next-line */
import 'bootstrap/scss/bootstrap-flex.scss';
/* eslint-disable-next-line */
import '../css/main.scss';

class Layout extends Component {
    constructor(props) {
        super(props);
        this.currentVersion = getCurrentVersion();
        this.platform = getPlatform();

        this.defaultState = defaultState;

        this.defaultColors = defaultColors;

        this.ignoreKeys = ['showDebug'];

        this.colorKeys = Object.keys(this.defaultColors);

        let newState = this.filterValidKeys(this.props.state, [...this.colorKeys, ...Object.keys(this.defaultState)]);

        this.moduleStateKeys = ['slotA', 'slotB', 'slotC', 'slotD', 'slotE', 'slotF'];
        this.moduleSleepStateKeys = [
            'sleepSlotA',
            'sleepSlotB',
            'sleepSlotC',
            'sleepSlotD',
            'sleepSlotE',
            'sleepSlotF',
        ];
        this.moduleTapStateKeys = ['tapSlotA', 'tapSlotB', 'tapSlotC', 'tapSlotD', 'tapSlotE', 'tapSlotF'];
        this.moduleWristStateKeys = [
            'wristSlotA',
            'wristSlotB',
            'wristSlotC',
            'wristSlotD',
            'wristSlotE',
            'wristSlotF',
        ];

        this.state = { ...this.defaultState, ...this.defaultColors, ...newState };
        this._ = getText.bind(this, getLocaleById(this.state.locale));

        this.modulesAll = [
            { value: '0', label: this._('None') },
            { value: '1', label: this._('Current Weather') },
            { value: '2', label: this._('Min/Max Temp') },
            { value: '3', label: this._('Steps') },
            { value: '4', label: this._('Distance') },
            { value: '5', label: this._('Calories') },
            { value: '6', label: this._('Sleep Time') },
            { value: '7', label: this._('Deep Sleep Time') },
            { value: '8', label: this._('Wind dir./speed') },
            { value: '11', label: this._('Sunrise') },
            { value: '12', label: this._('Sunset') },
            { value: '13', label: this._('Active time') },
            { value: '14', label: this._('Heart rate') },
            ...(this.platform !== 'diorite' ? [{ value: '15', label: this._('Compass') }] : []),
            { value: '16', label: this._('Seconds') },
            { value: '17', label: this._('Battery level') },
        ];

        this.modulesAplite = [
            { value: '0', label: this._('None') },
            { value: '1', label: this._('Current Weather') },
            { value: '2', label: this._('Min/Max Temp') },
            { value: '8', label: this._('Wind dir./speed') },
            { value: '11', label: this._('Sunrise') },
            { value: '12', label: this._('Sunset') },
            { value: '15', label: this._('Compass') },
            { value: '16', label: this._('Seconds') },
            { value: '17', label: this._('Battery level') },
        ];

        this.textModulesAll = [
            { value: '0', label: this._('None') },
            { value: '3', label: this._('Steps') },
            { value: '4', label: this._('Distance') },
            { value: '5', label: this._('Calories') },
            { value: '6', label: this._('Sleep Time') },
            { value: '7', label: this._('Deep Sleep Time') },
            { value: '13', label: this._('Active time') },
            { value: '16', label: this._('Seconds') },
            { value: '17', label: this._('Battery level') },
            { value: '18', label: this._('Alternate time zone') },
        ];

        this.textModulesAplite = [
            { value: '0', label: this._('None') },
            { value: '16', label: this._('Seconds') },
            { value: '17', label: this._('Battery level') },
            { value: '18', label: this._('Alternate time zone') },
        ];

        this.timezones = allTimezones;

        this.locales = [
            { value: '0', label: this._('English') },
            { value: '1', label: this._('Portuguese') },
            { value: '2', label: this._('French') },
            { value: '3', label: this._('German') },
            { value: '4', label: this._('Spanish') },
            { value: '5', label: this._('Italian') },
            { value: '6', label: this._('Dutch') },
            { value: '7', label: this._('Danish') },
            { value: '8', label: this._('Turkish') },
            { value: '9', label: this._('Czech') },
            { value: '10', label: this._('Polish') },
            { value: '11', label: this._('Swedish') },
            { value: '12', label: this._('Finnish') },
            { value: '13', label: this._('Slovak') },
        ];

        this.fonts = [
            { value: '0', label: 'Blocko' },
            { value: '1', label: 'Bloco (big)' },
            { value: '3', label: 'Archivo' },
            { value: '4', label: 'Din' },
            { value: '5', label: 'Prototype' },
            { value: '6', label: 'LECO' },
            { value: '7', label: 'Konstruct' },
        ];

        this.dateFormatOptions = [
            { value: '0', label: this._('Day of week, month, day') },
            { value: '1', label: this._('Day of week, day, month') },
            { value: '2', label: this._('Day of week, day') },
            { value: '3', label: this._('Day, month') },
            { value: '4', label: this._('Month, day') },
            { value: '5', label: this._('Day, month (number)') },
            { value: '6', label: this._('Month (number), day') },
            { value: '7', label: this._('Day of week, day, month (number)') },
            { value: '8', label: this._('Day of week, month (number), day') },
            { value: '9', label: this._('ISO-8601 (year, month, day)') },
            { value: '10', label: this._('Week number, day, month (number)') },
            { value: '11', label: this._('Week number, month (number), day') },
        ];

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
            window.localStorage.clear();
            this.setState({ ...this.defaultState, ...this.defaultColors });
        }
    };

    getStoredPresets = () => {
        let presets = this.state.presets || {};
        presets =
            Object.keys(presets).length > 0
                ? presets
                : {
                    ...JSON.parse(window.localStorage.presets || '{}'),
                    ...this.getOldPresets(),
                };
        return presets;
    };

    getOldPresets = () => {
        return Object.keys(localStorage).reduce((presets, key) => {
            if (key.indexOf('preset-') === 0) {
                let presetName = key.replace('preset-', '');
                let newPreset = JSON.parse(localStorage[key]);
                newPreset = Object.keys(newPreset).reduce((preset, key) => {
                    let value = newPreset[key];

                    value = value === 'true' || value === 'false' ? JSON.parse(value) : value;
                    value = typeof value === 'string' && value.indexOf('0x') !== -1 ? value.replace('0x', '#') : value;

                    preset[key] = value;

                    return preset;
                }, {});
                presets[presetName] = newPreset;
                delete localStorage[key];
            }
            return presets;
        }, {});
    };

    storePresets = (newPresets) => {
        this.setState({
            presets: newPresets,
        });
    };

    getChildContext() {
        return { locale: getLocaleById(this.state.locale) };
    }

    onChange = (key, value) => {
        console.log({ [key]: value });
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
            (this.weatherProviderSelected('0') && !this.state.openWeatherKey) ||
            (this.weatherProviderSelected('1') && !this.state.weatherKey) ||
            (this.weatherProviderSelected('3') && !this.state.forecastKey)
        ) {
            alert(this._('Please enter a valid API key for the selected provider'));
            return;
        }

        if (this.state.heartHigh && isNaN(this.state.heartHigh)) {
            alert(this._('Value for high heart rate limit should be a number'));
            return;
        }

        if (this.state.heartLow && isNaN(this.state.heartLow)) {
            alert(this._('Value for low heart rate limit should be a number'));
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

    weatherProviderSelected = (index) => {
        return [index].indexOf(this.state.weatherProvider) !== -1;
    };

    componentWillUpdate = () => {
        this._ = getText.bind(this, getLocaleById(this.state.locale));
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

    getModules = (options) => {
        return (
            <div>
                {options.map((item) => {
                    if (Array.isArray(item)) {
                        return (
                            <SideBySideFields>
                                <DropdownField
                                    fieldName={item[0].name}
                                    label={item[0].label}
                                    options={item[0].textOnly ? this.textModules : this.modules}
                                    searchable={false}
                                    clearable={false}
                                    labelPosition={item[0].labelPosition}
                                    selectedItem={this.state[item[0].slot]}
                                    onChange={this.onChangeDropdown.bind(this, item[0].slot)}
                                />
                                <DropdownField
                                    fieldName={item[1].name}
                                    label={item[1].label}
                                    options={item[1].textOnly ? this.textModules : this.modules}
                                    searchable={false}
                                    clearable={false}
                                    labelPosition={item[1].labelPosition}
                                    selectedItem={this.state[item[1].slot]}
                                    onChange={this.onChangeDropdown.bind(this, item[1].slot)}
                                />
                            </SideBySideFields>
                        );
                    } else {
                        return (
                            <DropdownField
                                fieldName={item.name}
                                label={item.label}
                                options={item.textOnly ? this.textModules : this.modules}
                                searchable={false}
                                clearable={false}
                                labelPosition={item.labelPosition}
                                selectedItem={this.state[item.slot]}
                                onChange={this.onChangeDropdown.bind(this, item.slot)}
                            />
                        );
                    }
                })}
            </div>
        );
    };

    getEnabledModules = () => {
        let state = this.state;

        let modules = {
            Default: this.getModules([
                [
                    {
                        name: 'top-left',
                        label: this._('Top Left'),
                        slot: 'slotA',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                    {
                        name: 'top-right',
                        label: this._('Top Right'),
                        slot: 'slotB',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                ],
                {
                    name: 'center-top',
                    label: this._('Center Top'),
                    slot: 'slotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom',
                    label: this._('Center Bottom'),
                    slot: 'slotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                [
                    {
                        name: 'bottom-left',
                        label: this._('Bottom Left'),
                        slot: 'slotC',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                    {
                        name: 'bottom-right',
                        label: this._('Bottom Right'),
                        slot: 'slotD',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                ],
            ]),
        };

        if (state.showSleep) {
            modules['Sleep'] = this.getModules([
                [
                    {
                        name: 'top-left-sleep',
                        label: this._('Top Left'),
                        slot: 'sleepSlotA',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                    {
                        name: 'top-right-sleep',
                        label: this._('Top Right'),
                        slot: 'sleepSlotB',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                ],
                {
                    name: 'center-top-sleep',
                    label: this._('Center Top'),
                    slot: 'sleepSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-sleep',
                    label: this._('Center Bottom'),
                    slot: 'sleepSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                [
                    {
                        name: 'bottom-left-sleep',
                        label: this._('Bottom Left'),
                        slot: 'sleepSlotC',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                    {
                        name: 'bottom-right-sleep',
                        label: this._('Bottom Right'),
                        slot: 'sleepSlotD',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                ],
            ]);
        }

        if (state.showTap) {
            modules['Tap'] = this.getModules([
                [
                    {
                        name: 'top-left-tap',
                        label: this._('Top Left'),
                        slot: 'tapSlotA',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                    {
                        name: 'top-right-tap',
                        label: this._('Top Right'),
                        slot: 'tapSlotB',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                ],
                {
                    name: 'center-top-tap',
                    label: this._('Center Top'),
                    slot: 'tapSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-tap',
                    label: this._('Center Bottom'),
                    slot: 'tapSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                [
                    {
                        name: 'bottom-left-tap',
                        label: this._('Bottom Left'),
                        slot: 'tapSlotC',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                    {
                        name: 'bottom-right-tap',
                        label: this._('Bottom Right'),
                        slot: 'tapSlotD',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                ],
            ]);
        }

        if (state.showWrist) {
            modules['Shake'] = this.getModules([
                [
                    {
                        name: 'top-left-wrist',
                        label: this._('Top Left'),
                        slot: 'wristSlotA',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                    {
                        name: 'top-right-wrist',
                        label: this._('Top Right'),
                        slot: 'wristSlotB',
                        textOnly: false,
                        labelPosition: 'top',
                    },
                ],
                {
                    name: 'center-top-wrist',
                    label: this._('Center Top'),
                    slot: 'wristSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-wrist',
                    label: this._('Center Bottom'),
                    slot: 'wristSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                [
                    {
                        name: 'bottom-left-wrist',
                        label: this._('Bottom Left'),
                        slot: 'wristSlotC',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                    {
                        name: 'bottom-right-wrist',
                        label: this._('Bottom Right'),
                        slot: 'wristSlotD',
                        textOnly: false,
                        labelPosition: 'bottom',
                    },
                ],
            ]);
        }

        return modules;
    };

    getModulesRound = (options) => {
        return (
            <div>
                {options.map((item) => {
                    return (
                        <DropdownField
                            key={item.label}
                            fieldName={item.name}
                            label={item.label}
                            options={item.textOnly ? this.textModules : this.modules}
                            searchable={false}
                            clearable={false}
                            labelPosition={item.labelPosition}
                            selectedItem={this.state[item.slot]}
                            onChange={this.onChangeDropdown.bind(this, item.slot)}
                        />
                    );
                })}
            </div>
        );
    };

    getEnabledModulesRound = () => {
        let state = this.state;

        let modules = {
            Default: this.getModulesRound([
                {
                    name: 'top-left',
                    label: this._('Top 1'),
                    slot: 'slotA',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'top-right',
                    label: this._('Top 2'),
                    slot: 'slotB',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'center-top',
                    label: this._('Center top'),
                    slot: 'slotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom',
                    label: this._('Center bottom'),
                    slot: 'slotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-left',
                    label: this._('Bottom 1'),
                    slot: 'slotC',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-right',
                    label: this._('Bottom 2'),
                    slot: 'slotD',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
            ]),
        };

        if (state.showSleep) {
            modules['Sleep'] = this.getModulesRound([
                {
                    name: 'top-left-sleep',
                    label: this._('Top 1'),
                    slot: 'sleepSlotA',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'top-right-sleep',
                    label: this._('Top 2'),
                    slot: 'sleepSlotB',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'center-top-sleep',
                    label: this._('Center top'),
                    slot: 'sleepSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-sleep',
                    label: this._('Center bottom'),
                    slot: 'sleepSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-left-sleep',
                    label: this._('Bottom 1'),
                    slot: 'sleepSlotC',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-right-sleep',
                    label: this._('Bottom 2'),
                    slot: 'sleepSlotD',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
            ]);
        }

        if (state.showTap) {
            modules['Tap'] = this.getModulesRound([
                {
                    name: 'top-left-tap',
                    label: this._('Top 1'),
                    slot: 'tapSlotA',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'top-right-tap',
                    label: this._('Top 2'),
                    slot: 'tapSlotB',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'center-top-tap',
                    label: this._('Center top'),
                    slot: 'tapSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-tap',
                    label: this._('Center bottom'),
                    slot: 'tapSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-left-tap',
                    label: this._('Bottom 1'),
                    slot: 'tapSlotC',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-right-tap',
                    label: this._('Bottom 2'),
                    slot: 'tapSlotD',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
            ]);
        }

        if (state.showWrist) {
            modules['Shake'] = this.getModulesRound([
                {
                    name: 'top-left-wrist',
                    label: this._('Top 1'),
                    slot: 'wristSlotA',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'top-right-wrist',
                    label: this._('Top 2'),
                    slot: 'wristSlotB',
                    textOnly: false,
                    labelPosition: 'top',
                },
                {
                    name: 'center-top-wrist',
                    label: this._('Center top'),
                    slot: 'wristSlotE',
                    textOnly: true,
                    labelPosition: 'top',
                },
                {
                    name: 'center-bottom-wrist',
                    label: this._('Center bottom'),
                    slot: 'wristSlotF',
                    textOnly: true,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-left-wrist',
                    label: this._('Bottom 1'),
                    slot: 'wristSlotC',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
                {
                    name: 'bottom-right-wrist',
                    label: this._('Bottom 2'),
                    slot: 'wristSlotD',
                    textOnly: false,
                    labelPosition: 'bottom',
                },
            ]);
        }

        return modules;
    };

    render() {
        let state = this.state;
        return (
            <div>
                <h1 className="title">Timeboxed</h1>
                <VersionIndicator onClick={this.toggleDebug} />

                {state.showDebug && (
                    <OptionGroup title={this._('Debug')}>
                        <ul className="debug-list">
                            {Object.keys(window.localStorage).map((key) => {
                                return (
                                    <li key={key}>
                                        {key}: {window.localStorage[key]}
                                    </li>
                                );
                            })}
                        </ul>
                        <button className="btn btn-danger" onClick={this.wipeConfigs}>
                            Reset configs
                        </button>
                    </OptionGroup>
                )}

                <OptionGroup title={this._('General')}>
                    {this.platform !== 'chalk' && (
                        <RadioButtonGroup
                            fieldName="textAlign"
                            label="Text Align"
                            options={[
                                { value: '0', label: this._('Left') },
                                { value: '1', label: this._('Center') },
                                { value: '2', label: this._('Right') },
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
                        label={this._('Hours with leading zero')}
                        checked={state.leadingZero}
                        onChange={this.onChange.bind(this, 'leadingZero')}
                    />
                    <ToggleField
                        fieldName="bluetoothDisconnect"
                        label={this._('Vibrate on Bluetooth disconnect')}
                        checked={state.bluetoothDisconnect}
                        onChange={this.onChange.bind(this, 'bluetoothDisconnect')}
                    />
                    <ToggleField
                        fieldName="muteOnQuiet"
                        label={this._('Mute vibrations on Quiet Mode')}
                        checked={state.muteOnQuiet}
                        onChange={this.onChange.bind(this, 'muteOnQuiet')}
                    />
                    <ToggleField
                        fieldName="updates"
                        label={this._('Check for updates')}
                        checked={state.update}
                        onChange={this.onChange.bind(this, 'update')}
                    />

                    {this.platform !== 'chalk' &&
                        this.plaform !== 'aplite' && (
                            <div>
                                <ToggleField
                                    fieldName="quickview"
                                    label={this._('Enable Quickview mode')}
                                    checked={state.quickview}
                                    onChange={this.onChange.bind(this, 'quickview')}
                                />
                                <HelperText>
                                    {this._(
                                        'Hides additional timezone and battery level and adjusts the layout when a timeline event is on the screen.'
                                    )}
                                </HelperText>
                            </div>
                        )}
                </OptionGroup>

                <OptionGroup title={this._('Modules')}>
                    <TabContainer
                        tabs={this.platform === 'chalk' ? this.getEnabledModulesRound() : this.getEnabledModules()}
                    />
                    {this.isEnabled(['15']) &&
                        !this.isEnabledTapWrist(['15']) && (
                            <HelperText standalone={true}>
                                {this._(
                                    '<strong>Alert:</strong> Keeping the compass enabled all the time could drain battery faster. It\'s recommend setting it as a \'Tap\' or \'Shake\' module (enable them below).'
                                )}
                            </HelperText>
                        )}
                    {this.platform !== 'aplite' && (
                        <div>
                            <ToggleField
                                fieldName="showSleep"
                                label={this._('Enable after wake up mode')}
                                checked={state.showSleep}
                                onChange={this.onChange.bind(this, 'showSleep')}
                            />
                            <HelperText>
                                {this._(
                                    'If set, the watchface will show the modules under the \'Sleep\' tab from when you\'re asleep until half an hour after you wake up, switching back to the \'Default\' tab after that. This feature requires Pebble Health enabled.'
                                )}
                            </HelperText>
                            <ToggleField
                                fieldName="showTap"
                                label={this._('Enable tap mode')}
                                checked={state.showTap}
                                onChange={this.onChange.bind(this, 'showTap')}
                            />
                            <ToggleField
                                fieldName="showWrist"
                                label={this._('Enable wrist shake mode')}
                                checked={state.showWrist}
                                onChange={this.onChange.bind(this, 'showWrist')}
                            />
                            <HelperText>
                                {this._(
                                    '<strong>Experimental features:</strong> If set, the watchface will show the modules under the \'Tap\' tab when you tap the watch screen or the modules under \'Shake\' when you shake your wrist for the amount of time selected below , switching back to the previous view after that. Keep in mind that tap detection is a bit rudimentary because of pebble\'s accelerometer, so light taps might not work. <strong>Enabling these features might drain the battery faster than usual.</strong>'
                                )}
                            </HelperText>
                            {(this.state.showTap || this.state.showWrist) && (
                                <RadioButtonGroup
                                    fieldName="tapTime"
                                    label="Tap/Shake mode duration"
                                    options={[
                                        { value: '5', label: this._('5s') },
                                        { value: '7', label: this._('7s') },
                                        { value: '10', label: this._('10s') },
                                    ]}
                                    selectedItem={state.tapTime}
                                    onChange={this.onChange.bind(this, 'tapTime')}
                                />
                            )}
                        </div>
                    )}
                </OptionGroup>

                {this.isEnabled(['18']) && (
                    <OptionGroup title={this._('Alternate Timezone')}>
                        <DropdownField
                            fieldName="timezones"
                            label={this._('Additional Timezone')}
                            options={this.timezones}
                            searchable={true}
                            clearable={false}
                            selectedItem={state.timezones}
                            onChange={this.onChangeDropdown.bind(this, 'timezones')}
                        />
                    </OptionGroup>
                )}

                <OptionGroup title={this._('Localization')}>
                    <DropdownField
                        fieldName="locale"
                        label={this._('Language')}
                        options={this.locales}
                        searchable={true}
                        clearable={false}
                        selectedItem={state.locale}
                        onChange={this.onChangeDropdown.bind(this, 'locale')}
                    />
                    <DropdownField
                        fieldName="dateFormat"
                        label={this._('Date format')}
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
                            { value: '0', label: this._('(space)') },
                            { value: '1', label: this._('.') },
                            { value: '2', label: this._('/') },
                            { value: '3', label: this._('-') },
                        ]}
                        selectedItem={state.dateSeparator}
                        onChange={this.onChange.bind(this, 'dateSeparator')}
                    />
                </OptionGroup>

                <OptionGroup title={this._('Colors')}>
                    <ColorPicker
                        fieldName="backgroundColor"
                        label={this._('Background color')}
                        color={state.bgColor}
                        onChange={this.onChange.bind(this, 'bgColor')}
                    />
                    <ColorPicker
                        fieldName="textColor"
                        label={this._('Foreground color')}
                        color={state.hoursColor}
                        onChange={this.onChange.bind(this, 'hoursColor')}
                    />
                    <ToggleField
                        fieldName="enableAdvanced"
                        label={this._('Advanced Colors')}
                        checked={state.enableAdvanced}
                        onChange={this.onChange.bind(this, 'enableAdvanced')}
                    />
                    <HelperText>
                        {this._(
                            'Enables specific color configuration for watchface items. If disabled, all text will have the same color as the selection for \'Foreground Color\' above. This also lets you save and load color presets.'
                        )}
                    </HelperText>
                    {state.enableAdvanced && (
                        <div>
                            <ColorPicker
                                fieldName="dateColor"
                                label={this._('Date color')}
                                color={state.dateColor}
                                onChange={this.onChange.bind(this, 'dateColor')}
                            />
                            {this.isEnabled(['18']) && (
                                <ColorPicker
                                    fieldName="altHoursColor"
                                    label={this._('Alternate timezone color')}
                                    color={state.altHoursColor}
                                    onChange={this.onChange.bind(this, 'altHoursColor')}
                                />
                            )}
                            {this.isEnabled(['17']) && (
                                <ColorPicker
                                    fieldName="batteryColor"
                                    label={this._('Battery/Low Battery color')}
                                    color={state.batteryColor}
                                    onChange={this.onChange.bind(this, 'batteryColor')}
                                    secondColor={state.batteryLowColor}
                                    onSecondColorChange={this.onChange.bind(this, 'batteryLowColor')}
                                />
                            )}
                            <ColorPicker
                                fieldName="bluetoothColor"
                                label={this._('Bluetooth disconnected')}
                                color={state.bluetoothColor}
                                onChange={this.onChange.bind(this, 'bluetoothColor')}
                            />
                            <ColorPicker
                                fieldName="updateColor"
                                label={this._('Update notification')}
                                color={state.updateColor}
                                onChange={this.onChange.bind(this, 'updateColor')}
                            />
                            {this.isEnabled(['1']) && (
                                <ColorPicker
                                    fieldName="weatherColor"
                                    label={this._('Weather icon/temperature')}
                                    color={state.weatherColor}
                                    onChange={this.onChange.bind(this, 'weatherColor')}
                                    secondColor={state.tempColor}
                                    onSecondColorChange={this.onChange.bind(this, 'tempColor')}
                                />
                            )}
                            {this.isEnabled(['2']) && (
                                <ColorPicker
                                    fieldName="minMaxTemp"
                                    label={this._('Min/Max temperature')}
                                    color={state.minColor}
                                    onChange={this.onChange.bind(this, 'minColor')}
                                    secondColor={state.maxColor}
                                    onSecondColorChange={this.onChange.bind(this, 'maxColor')}
                                />
                            )}
                            {this.isEnabled(['3']) && (
                                <ColorPicker
                                    fieldName="stepsColor"
                                    label={this._('Steps/falling behind')}
                                    color={state.stepsColor}
                                    onChange={this.onChange.bind(this, 'stepsColor')}
                                    secondColor={state.stepsBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'stepsBehindColor')}
                                />
                            )}
                            {this.isEnabled(['4']) && (
                                <ColorPicker
                                    fieldName="distColor"
                                    label={this._('Distance/falling behind')}
                                    color={state.distColor}
                                    onChange={this.onChange.bind(this, 'distColor')}
                                    secondColor={state.distBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'distBehindColor')}
                                />
                            )}
                            {this.isEnabled(['5']) && (
                                <ColorPicker
                                    fieldName="calColor"
                                    label={this._('Calories/falling behind')}
                                    color={state.calColor}
                                    onChange={this.onChange.bind(this, 'calColor')}
                                    secondColor={state.calBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'calBehindColor')}
                                />
                            )}
                            {this.isEnabled(['6']) && (
                                <ColorPicker
                                    fieldName="sleepColor"
                                    label={this._('Sleep/falling behind')}
                                    color={state.sleepColor}
                                    onChange={this.onChange.bind(this, 'sleepColor')}
                                    secondColor={state.sleepBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'sleepBehindColor')}
                                />
                            )}
                            {this.isEnabled(['7']) && (
                                <ColorPicker
                                    fieldName="deepColor"
                                    label={this._('Deep sleep/falling behind')}
                                    color={state.deepColor}
                                    onChange={this.onChange.bind(this, 'deepColor')}
                                    secondColor={state.deepBehindColor}
                                    onSecondColorChange={this.onChange.bind(this, 'deepBehindColor')}
                                />
                            )}
                            {this.isEnabled(['8']) && (
                                <ColorPicker
                                    fieldName="windSpeedColor"
                                    label={this._('Wind direction/speed')}
                                    color={state.windDirColor}
                                    onChange={this.onChange.bind(this, 'windDirColor')}
                                    secondColor={state.windSpeedColor}
                                    onSecondColorChange={this.onChange.bind(this, 'windSpeedColor')}
                                />
                            )}
                            {this.isEnabled(['11']) && (
                                <ColorPicker
                                    fieldName="sunriseColor"
                                    label={this._('Sunrise')}
                                    color={state.sunriseColor}
                                    onChange={this.onChange.bind(this, 'sunriseColor')}
                                />
                            )}
                            {this.isEnabled(['12']) && (
                                <ColorPicker
                                    fieldName="sunsetColor"
                                    label={this._('Sunset')}
                                    color={state.sunsetColor}
                                    onChange={this.onChange.bind(this, 'sunsetColor')}
                                />
                            )}
                            {this.isEnabled(['13']) && (
                                <ColorPicker
                                    fieldName="activeColor"
                                    label={this._('Active time/falling behind')}
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
                                        label={this._('Heart rate/outside limits')}
                                        color={state.heartColor}
                                        onChange={this.onChange.bind(this, 'heartColor')}
                                        secondColor={state.heartColorOff}
                                        onSecondColorChange={this.onChange.bind(this, 'heartColorOff')}
                                    />
                                    <HelperText>
                                        {this._('Set upper and lower limits in the health section below.')}
                                    </HelperText>
                                </div>
                            )}
                            {this.isEnabled(['15']) && (
                                <ColorPicker
                                    fieldName="compassColor"
                                    label={this._('Compass')}
                                    color={state.compassColor}
                                    onChange={this.onChange.bind(this, 'compassColor')}
                                />
                            )}
                            {this.isEnabled(['16']) && (
                                <ColorPicker
                                    fieldName="secondsColor"
                                    label={this._('Seconds')}
                                    color={state.secondsColor}
                                    onChange={this.onChange.bind(this, 'secondsColor')}
                                />
                            )}
                        </div>
                    )}
                </OptionGroup>

                {state.enableAdvanced && (
                    <OptionGroup title={this._('Color Presets')}>
                        <ColorPresets
                            colors={this.getCurrentColors()}
                            onSelect={this.onPresetSelect}
                            storePresets={this.storePresets}
                            presets={state.presets}
                        />
                    </OptionGroup>
                )}

                {this.isWeatherEnabled() && (
                    <OptionGroup title={this._('Weather')}>
                        <DropdownField
                            fieldName="provider"
                            label={this._('Weather provider')}
                            options={[
                                { value: '0', label: 'OpenWeatherMap' },
                                { value: '1', label: 'WeatherUnderground' },
                                { value: '2', label: 'Yahoo' },
                                { value: '3', label: 'Dark Sky/Forecast.io' },
                            ]}
                            selectedItem={state.weatherProvider}
                            onChange={this.onChangeDropdown.bind(this, 'weatherProvider')}
                        />

                        {this.weatherProviderSelected('0') && (
                            <APIKey
                                keyName="openWeatherKey"
                                value={state.openWeatherKey}
                                onChange={this.onChange.bind(this, 'openWeatherKey')}
                                helperText={this._(
                                    '<strong>Note:</strong> Unfortunately, the API key previously used by Timeboxed couldn\'t handle all the users, so from now you need your own API key. Go to <a href="https://home.openweathermap.org/users/sign_up">home.openweathermap.org/users/sign_up</a> to create a free account and get a key and insert it above.'
                                )}
                            />
                        )}
                        {this.weatherProviderSelected('1') && (
                            <APIKey
                                keyName="weatherKey"
                                value={state.weatherKey}
                                onChange={this.onChange.bind(this, 'weatherKey')}
                                helperText={this._(
                                    '<strong>Note:</strong> For WeatherUnderground, you need an API key. Go to <a href="http://www.wunderground.com/weather/api/?apiref=73d2b41a1a02e3bd">wunderground.com</a> to create a free account and get a key and insert it above.'
                                )}
                            />
                        )}
                        {this.weatherProviderSelected('3') && (
                            <APIKey
                                keyName="forecastKey"
                                value={state.forecastKey}
                                onChange={this.onChange.bind(this, 'forecastKey')}
                                helperText={this._(
                                    '<strong>Note:</strong> For Dark Sky/Forecast.io, you need an API key. Go to <a href="https://darksky.net/dev/">darksky.net/dev/</a> to create a free account and get a key and insert it above.'
                                )}
                            />
                        )}

                        {this.isEnabled(['1', '2']) && (
                            <ToggleField
                                fieldName="useCelsius"
                                label={this._('Use Celsius')}
                                checked={state.useCelsius}
                                onChange={this.onChange.bind(this, 'useCelsius')}
                            />
                        )}

                        {this.isEnabled(['1', '2', '8', '11', '12']) && (
                            <RadioButtonGroup
                                fieldName="weatherTime"
                                label={this._('Weather refresh interval')}
                                options={[
                                    { value: '10', label: '10min' },
                                    { value: '15', label: '15min' },
                                    { value: '30', label: '30min' },
                                ]}
                                selectedItem={state.weatherTime}
                                onChange={this.onChange.bind(this, 'weatherTime')}
                            />
                        )}

                        {this.isEnabled(['8']) && (
                            <RadioButtonGroup
                                fieldName="speedUnit"
                                label={this._('Speed unit')}
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
                            buttonLabel={this._('Verify')}
                            label={this._('Manual Location')}
                            labelPosition="top"
                            value={state.overrideLocation}
                            onButtonClick={this.verifyLocation}
                            onChange={this.onChange.bind(this, 'overrideLocation')}
                        />
                        <HelperText>
                            {this._(
                                'If you define a manual location, we won\'t try to use your current location for weather info. (max. length 64 characters). <strong>Note</strong>: If you\'re not sure the city you entered is working, use the \'Verify\' button to check if it\'s a valid location.'
                            )}
                        </HelperText>
                    </OptionGroup>
                )}

                <OptionGroup title={this._('Master Key (pmkey.xyz)')}>
                    <TextField
                        fieldName="masterKeyEmail"
                        label={this._('Email')}
                        value={state.masterKeyEmail}
                        onChange={this.onChange.bind(this, 'masterKeyEmail')}
                    />
                    <TextField
                        fieldName="masterKeyPin"
                        label={this._('Pin')}
                        value={state.masterKeyPin}
                        onChange={this.onChange.bind(this, 'masterKeyPin')}
                    />
                    <HelperText>
                        {this._(
                            'Enter your email and pin to retrieve your API keys stored with Master Key. We read the WeatherUnderground and/or the DarkSky/Forecast.io API keys.'
                        )}
                    </HelperText>
                    <button onClick={this.getMasterKeyData} className="btn btn-primary">
                        {this._('Retrieve API keys')}
                    </button>
                </OptionGroup>
                {this.isEnabled(['14']) && (
                    <OptionGroup title={this._('Health')}>
                        <TextField
                            fieldName="heartLow"
                            label={this._('Lower heart rate limit')}
                            value={state.heartLow}
                            onChange={this.onChange.bind(this, 'heartLow')}
                        />
                        <TextField
                            fieldName="heartHigh"
                            label={this._('Upper heart rate limit')}
                            value={state.heartHigh}
                            onChange={this.onChange.bind(this, 'heartHigh')}
                        />
                        <HelperText>
                            {this._(
                                'If any of the values are set and different than zero we\'ll show the heart rate in a different color when it\'s below the lower limit or above the upper limit.'
                            )}
                        </HelperText>
                    </OptionGroup>
                )}

                <div className="block--footer">
                    <HelperText standalone={true}>{this._('Remember to save to apply your settings.')}</HelperText>
                    <HelperText standalone={true}>
                        {this._(
                            'Fonts: <a href="http://www.dafont.com/blocko.font">Blocko</a>, <a href="https://fontlibrary.org/en/font/osp-din">OSP-DIN</a>, <a href="https://www.google.com/fonts/specimen/Archivo+Narrow">Archivo Narrow</a> and <a href="http://www.dafont.com/prototype.font">Prototype</a>.<br />Weather font used: <a href="https://erikflowers.github.io/weather-icons/">Erik Flower\'s Weather Icons</a>.'
                        )}
                    </HelperText>
                    <HelperText standalone={true}>
                        {this._('If you like Timeboxed, please consider donating ;)')}
                    </HelperText>
                    <DonateButton />

                    <div className="block--submit">
                        <button onClick={this.onSubmit} className="btn btn-primary btn-lg btn-submit">
                            {this._('Save settings')}
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
