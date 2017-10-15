import { getPlatform } from './util/util';
import FastClick from 'react-fastclick-alt';
import RadioButtonGroup from './button-group';
import React, { Component, PropTypes } from 'react';

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

        this.onColorTypeChange = this.onColorTypeChange.bind(this);
    }

    onColorTypeChange(colorType) {
        this.setState({ sunny: parseInt(colorType, 10) === 1 });
    }

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

class SwatchItem extends Component {
    constructor(props) {
        super(props);

        this.onClickHandler = this.onClickHandler.bind(this);
    }

    onClickHandler() {
        if (this.props.color) {
            this.props.onClick(this.props.color);
        }
    }

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

export default Swatches;
