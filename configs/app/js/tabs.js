import FastClick from 'react-fastclick-alt';
import React, { Component, PropTypes } from 'react';
import classnames from 'classnames';

class TabContainer extends Component {
    constructor(props) {
        super(props);
        this.state = {
            selectedTab: 0,
        };
    }

    onClick(selected) {
        this.setState({ selectedTab: selected });
        if (this.props.onChange) {
            this.props.onChange(selected);
        }
    }

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

export default TabContainer;
