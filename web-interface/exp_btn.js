class ExpBtn extends HTMLElement{
    constructor() {
        // Always call super first in constructor
        super();

        // this.dev_icon = `
        //     <svg slot="icon" viewBox="0 0 24 24">
        //         <path d="M8,5.14V19.14L19,12.14L8,5.14Z"/>
        //     </svg>
        // `;

        this.dev_icon = barrel_icon;
    }

    connectedCallback(){
        this.deviceName = this.getAttribute("deviceName");

        this.dev_icon = icon_map[this.deviceName.toLowerCase()] || this.dev_icon;

        const shadow = this.attachShadow({ mode: "open" });

        const btn_container = document.createElement('div');
        btn_container.className = "btn_container";
        btn_container.id = "btn_container";

        this.deviceButton = document.createElement('md-filled-button');
        this.deviceButton.textContent = this.deviceName;
        this.deviceButton.id = this.deviceName;
        this.deviceButton.className = "deviceButton";
        if(this.deviceName != "Controller"){
            this.deviceButton.onclick = () => selectDevice(this.deviceName);
        }
        this.deviceButton.innerHTML = this.deviceName + this.dev_icon;

        const optsButton = document.createElement('md-filled-icon-button');
        optsButton.setAttribute("open", "false");
        optsButton.setAttribute("toggle", "false");

        optsButton.innerHTML = `
            <md-icon>
                <svg viewBox="0 0 24 24">
                    <path d="M3,6H21V8H3V6M3,11H21V13H3V11M3,16H21V18H3V16Z"/>
                </svg>
            </md-icon>
            <md-icon slot="selected">
                <svg viewBox="0 0 24 24">
                    <path d="M19,6.41L17.59,5L12,10.59L6.41,5L5,6.41L10.59,12L5,17.59L6.41,19L12,13.41L17.59,19L19,17.59L13.41,12L19,6.41Z" />
                </svg>
            </md-icon>
        `;
        optsButton.id = "optsButton_" + this.deviceName;
        optsButton.className = "optsButton";

        const options = document.createElement("opts-menu");
        options.setAttribute("deviceName", this.deviceName);
        options.id = "options_" + this.deviceName;

        const style = document.createElement("style");
        style.textContent = `
            .btn_container{
                margin: 10px;
                width: max-content;
                /*height: 2em;*/
                /*position: relative;*/
            }
            .deviceButton{
                margin-right: 2.5px;
            }
            .warning {
                --md-sys-color-primary: rgb(227 195 108);
                --md-sys-color-surface-tint: rgb(227 195 108);
                --md-sys-color-on-primary: rgb(61 47 0);
                --md-sys-color-primary-container: rgb(88 68 0);
                --md-sys-color-on-primary-container: rgb(255 224 142);
                --md-sys-color-secondary: rgb(213 197 161);
                --md-sys-color-on-secondary: rgb(57 47 21);
                --md-sys-color-secondary-container: rgb(80 70 42);
                --md-sys-color-on-secondary-container: rgb(242 225 187);
                --md-sys-color-tertiary: rgb(173 207 173);
                --md-sys-color-on-tertiary: rgb(25 54 31);
                --md-sys-color-tertiary-container: rgb(48 77 52);
                --md-sys-color-on-tertiary-container: rgb(201 236 200);
                --md-sys-color-error: rgb(255 180 171);
                --md-sys-color-on-error: rgb(105 0 5);
                --md-sys-color-error-container: rgb(147 0 10);
                --md-sys-color-on-error-container: rgb(255 218 214);
                --md-sys-color-background: rgb(22 19 11);
                --md-sys-color-on-background: rgb(234 225 212);
                --md-sys-color-surface: rgb(22 19 11);
                --md-sys-color-on-surface: rgb(234 225 212);
                --md-sys-color-surface-variant: rgb(76 70 57);
                --md-sys-color-on-surface-variant: rgb(207 197 180);
                --md-sys-color-outline: rgb(152 144 128);
                --md-sys-color-outline-variant: rgb(76 70 57);
                --md-sys-color-shadow: rgb(0 0 0);
                --md-sys-color-scrim: rgb(0 0 0);
                --md-sys-color-inverse-surface: rgb(234 225 212);
                --md-sys-color-inverse-on-surface: rgb(52 48 39);
                --md-sys-color-inverse-primary: rgb(115 91 12);
                --md-sys-color-primary-fixed: rgb(255 224 142);
                --md-sys-color-on-primary-fixed: rgb(36 26 0);
                --md-sys-color-primary-fixed-dim: rgb(227 195 108);
                --md-sys-color-on-primary-fixed-variant: rgb(88 68 0);
                --md-sys-color-secondary-fixed: rgb(242 225 187);
                --md-sys-color-on-secondary-fixed: rgb(35 27 4);
                --md-sys-color-secondary-fixed-dim: rgb(213 197 161);
                --md-sys-color-on-secondary-fixed-variant: rgb(80 70 42);
                --md-sys-color-tertiary-fixed: rgb(201 236 200);
                --md-sys-color-on-tertiary-fixed: rgb(4 33 11);
                --md-sys-color-tertiary-fixed-dim: rgb(173 207 173);
                --md-sys-color-on-tertiary-fixed-variant: rgb(48 77 52);
                --md-sys-color-surface-dim: rgb(22 19 11);
                --md-sys-color-surface-bright: rgb(61 57 47);
                --md-sys-color-surface-container-lowest: rgb(17 14 7);
                --md-sys-color-surface-container-low: rgb(31 27 19);
                --md-sys-color-surface-container: rgb(35 31 23);
                --md-sys-color-surface-container-high: rgb(46 42 33);
                --md-sys-color-surface-container-highest: rgb(57 52 43);
            }
        `;

        shadow.appendChild(style);
        shadow.appendChild(btn_container);
        btn_container.appendChild(this.deviceButton);
        btn_container.appendChild(optsButton);
        btn_container.appendChild(options);

        this.addEventListener('opts-rendered', (event) => {
            console.log("Options rendered");

            // optsButton.addEventListener("click", () => {
            //     const optsEl = options.shadowRoot.querySelector("#options_" + deviceName);
            //     optsEl.open = !optsEl.open;
            // });
        });

        optsButton.onclick = () => {
            const optsEl = options.shadowRoot.querySelector("#options_" + this.deviceName);
            optsEl.open = !optsEl.open;
        }

        // this.addEventListener("close-menu", (event) => {
        //     optsButton.selected = !optsButton.selected;
        // });

        this.addEventListener("ariahiddenchange", (event) => {
            optsButton.selected = !(event.detail.newValue === "true");
        });

    }

    updateEMState(state){
        if(state){
            this.deviceButton.innerHTML = this.deviceName + warning_icon;
            this.deviceButton.className = "deviceButton warning";
        } else{
            this.deviceButton.innerHTML = this.deviceName + this.dev_icon;
            this.deviceButton.className = "deviceButton";
        }
    }

    static get observedAttributes() {
        return ["deviceName"];
    }
}

customElements.define('exp-btn', ExpBtn);