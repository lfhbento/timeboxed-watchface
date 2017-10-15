import { getLocaleById, getText } from './lang';
import React, { PropTypes } from 'react';

class DonateButton extends React.Component {
    constructor(props, context) {
        super(props, context);
        this._ = getText.bind(this, getLocaleById(this.context.locale));
        this.state = {
            text: this._('Donate'),
        };
        this.changeText = this.changeText.bind(this);
    }

    changeText() {
        this.setState({
            text: this._('Redirecting to paypal...'),
        });
    }

    render() {
        return (
            <form action="https://www.paypal.com/cgi-bin/webscr" method="post" target="_top">
                <input type="hidden" name="cmd" value="_s-xclick" />
                <input
                    type="hidden"
                    name="encrypted"
                    value="-----BEGIN PKCS7-----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-----END PKCS7-----"
                />
                <input
                    type="submit"
                    value={this.state.text}
                    onClick={this.changeText}
                    id="donateBtn"
                    className="btn btn-success btn-sm btn--donate"
                />
                <img alt="" border="0" src="https://www.paypalobjects.com/en_US/i/scr/pixel.gif" width="1" height="1" />
            </form>
        );
    }
}

DonateButton.childContextTypes = {
    locale: PropTypes.string,
};

export default DonateButton;
