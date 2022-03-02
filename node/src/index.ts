"use strict";

import fs from 'fs';
import path from 'path';

async function compile(text: string): Promise<Int32Array> {
    const buffer: (number | string)[] = [];

    const operation = /^([몰아\?루]{3})(\s+(\S+))?(\s+(\S+))?\s*/;
    const addression = /^ﾌﾄｽﾄ(\!*)\s*/;

    const immediator = (expr: string): number => {
        const table = {
            "몰": 0,
            "아": 1,
            "?": 2,
            "루": 3,
        };
        return expr.split('').reduce((prev: number, i: string): number => (prev << 2) + table[i], 0);
    };
    const typeParser = (expr: string | undefined): { type: number, value: number | string } => {
        if (expr == undefined) {
            return { type: -1, value: 0 };
        }
        let match: RegExpMatchArray;
        if (match = expr.match(/^[몰아\?루]+$/)) {
            return { type: 0, value: immediator(expr) }; // immediate
        } else if (match = expr.match(/^몰(\d)루$/)) {
            return { type: 1, value: +match[1] }; // register
        } else if (match = expr.match(/^ﾌﾄｽﾄ(\!*)$/)) {
            return { type: 2, value: match[1] }; // address
        } else if (match = expr.match(/^털(\!*)자$/)) {
            return { type: 3, value: match[1] }; // registerized address
        } else if (match = expr.match(/^털(\d)자$/)) {
            return { type: 4, value: +match[1] }; // registerized addresserized register
        } else {
            return { type: -1, value: 0 };
        }
    };

    const addresses: { [key: string]: number } = {};

    while (text) {
        text = text.trimStart();
        let match: RegExpMatchArray;
        if (match = text.match(operation)) {
            const operator = immediator(match[1]);
            const arg0 = typeParser(match[3]);
            const arg1 = typeParser(match[5]);

            switch (operator) {
                case 0: // NAND
                    if (arg0.type < 0 || arg0.type == 0 || arg0.type == 2 || arg1.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]} ${match[5]}" 몰?루`);
                    }
                    buffer.push(0 + arg0.type * 5 + arg1.type, arg0.value, arg1.value);
                    text = text.substring(match[1].length + match[2].length + match[4].length);
                    break;
                case 1: // SHL
                    if (arg0.type < 0 || arg0.type == 0 || arg0.type == 2 || arg1.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]} ${match[5]}" 몰?루`);
                    }
                    buffer.push(25 + arg0.type * 5 + arg1.type, arg0.value, arg1.value);
                    text = text.substring(match[1].length + match[2].length + match[4].length);
                    break;
                case 2: // SHR
                    if (arg0.type < 0 || arg0.type == 0 || arg0.type == 2 || arg1.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]} ${match[5]}" 몰?루`);
                    }
                    buffer.push(50 + arg0.type * 5 + arg1.type, arg0.value, arg1.value);
                    text = text.substring(match[1].length + match[2].length + match[4].length);
                    break;
                case 3: // MOV
                    if (arg0.type < 0 || arg0.type == 0 || arg0.type == 2 || arg1.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]} ${match[5]}" 몰?루`);
                    }
                    buffer.push(75 + arg0.type * 5 + arg1.type, arg0.value, arg1.value);
                    text = text.substring(match[1].length + match[2].length + match[4].length);
                    break;
                case 4: // CMP
                    if (arg0.type < 0 || arg1.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]} ${match[5]}" 몰?루`);
                    }
                    buffer.push(100 + arg0.type * 5 + arg1.type, arg0.value, arg1.value);
                    text = text.substring(match[1].length + match[2].length + match[4].length);
                    break;
                case 5: // JG
                    if (arg0.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(125 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                case 6: // JMP
                    if (arg0.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(130 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                case 7: // CALL
                    if (arg0.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(135 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                case 8: // RET
                    buffer.push(140);
                    text = text.substring(match[1].length);
                    break;
                case 9: // INT
                    if (arg0.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(141 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                    case 10: // PUSH
                    if (arg0.type < 0) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(146 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                    case 11: // NOP
                        buffer.push(0);
                        text = text.substring(match[1].length);
                        break;
                case 12: // POP
                    if (arg0.type < 0 || arg0.type == 0 || arg0.type == 3) {
                        throw new Error(`"${match[1]} ${match[3]}" 몰?루`);
                    }
                    buffer.push(151 + arg0.type, arg0.value);
                    text = text.substring(match[1].length + match[2].length);
                    break;
                case 27: // HLT
                    buffer.push(156);
                    text = text.substring(match[1].length);
                    break;
            }
        } else if (match = text.match(addression)) {
            addresses[match[1]] = buffer.length;
            text = text.substring(match[0].length);
        }
    }


    return new Int32Array(buffer.map(v => typeof (v) == "number" ? v : addresses[v] ? addresses[v] : Math.floor(Math.random() * buffer.length)) as number[]);
}

async function run(code: Int32Array, relative: string) {
    const buffer = new Int32Array(code);
    let cursor = 0;
    const registers = {
        0: Math.floor(Math.random() * 2147483647),
        1: Math.floor(Math.random() * 2147483647),
        2: Math.floor(Math.random() * 2147483647),
        3: Math.floor(Math.random() * 2147483647),
        4: Math.floor(Math.random() * 2147483647),
        5: Math.floor(Math.random() * 2147483647),
        6: Math.floor(Math.random() * 2147483647),
        7: Math.floor(Math.random() * 2147483647),
        8: Math.floor(Math.random() * 2147483647),
        9: Math.floor(Math.random() * 2147483647),
    };
    const stack: number[] = [];
    let state = false;
    const file = {
        path: "",
        content: "",
        cursor: 0
    };

    const evaluate = (arg: { type: number, value: number }): number => {
        switch (arg.type) {
            case 0:
                return arg.value;
            case 1:
                return registers[arg.value];
            case 2:
                return arg.value;
            case 3:
                return buffer[arg.value];
            case 4:
                return buffer[registers[arg.value]];
            default:
                return Math.floor(Math.random() * 2147483647);
        }
    };
    const insert = (arg: { type: number, value: number }, value: number) => {
        switch (arg.type) {
            case 1:
                registers[arg.value] = value;
                break;
            case 3:
                buffer[arg.value] = value;
                break;
            case 4:
                buffer[registers[arg.value]] = value;
                break;
        }
    };

    while (cursor < buffer.length) {
        if (buffer[cursor] < 25) { // NAND
            const arg0 = { type: Math.floor(buffer[cursor] / 5), value: buffer[cursor + 1] };
            const arg1 = { type: buffer[cursor] % 5, value: buffer[cursor + 2] };
            insert(arg0, ~(evaluate(arg0) & evaluate(arg1)));
            cursor += 3;
        } else if (buffer[cursor] < 50) { // SHL
            const arg0 = { type: Math.floor((buffer[cursor] - 25) / 5), value: buffer[cursor + 1] };
            const arg1 = { type: (buffer[cursor] - 25) % 5, value: buffer[cursor + 2] };
            insert(arg0, evaluate(arg0) << evaluate(arg1));
            cursor += 3;
        } else if (buffer[cursor] < 75) { // SHR
            const arg0 = { type: Math.floor((buffer[cursor] - 50) / 5), value: buffer[cursor + 1] };
            const arg1 = { type: (buffer[cursor] - 50) % 5, value: buffer[cursor + 2] };
            insert(arg0, evaluate(arg0) >> evaluate(arg1));
            cursor += 3;
        } else if (buffer[cursor] < 100) { // MOV
            const arg0 = { type: Math.floor((buffer[cursor] - 75) / 5), value: buffer[cursor + 1] };
            const arg1 = { type: (buffer[cursor] - 75) % 5, value: buffer[cursor + 2] };
            insert(arg0, evaluate(arg1));
            cursor += 3;
        } else if (buffer[cursor] < 125) { // CMP
            const arg0 = { type: Math.floor((buffer[cursor] - 100) / 5), value: buffer[cursor + 1] };
            const arg1 = { type: (buffer[cursor] - 100) % 5, value: buffer[cursor + 2] };
            const A = evaluate(arg0);
            const B = evaluate(arg1);
            state = A > B;
            cursor += 3;
        } else if (buffer[cursor] < 130) { // JG
            const arg0 = { type: buffer[cursor] - 125, value: buffer[cursor + 1] };
            if (state) {
                cursor = evaluate(arg0);
            } else {
                cursor += 2;
            }
        } else if (buffer[cursor] < 135) { // JMP
            const arg0 = { type: buffer[cursor] - 135, value: buffer[cursor + 1] };
            cursor = evaluate(arg0);
        } else if (buffer[cursor] < 140) { // CALL
            const arg0 = { type: buffer[cursor] - 140, value: buffer[cursor + 1] };
            stack.push(cursor + 2);
            cursor = evaluate(arg0);
        } else if (buffer[cursor] < 141) { // RET
            cursor = stack.pop();
        } else if (buffer[cursor] < 146) { // INT
            const arg0 = { type: buffer[cursor] - 146, value: buffer[cursor + 1] };
            switch (evaluate(arg0)) {
                case 0:
                    process.stdout.write(String.fromCharCode(registers[0]));
                    break;
                case 1:
                    await new Promise<void>(resolve => {
                        process.stdin.resume();
                        process.stdin.once('data', chunk => {
                            registers[0] = chunk[0];
                            process.stdin.pause();
                            resolve();
                        });
                    });
                    break;
                case 2:
                    await new Promise<void>(resolve => {
                        file.path = "";
                        for (let i = registers[1]; buffer[i]; i++) {
                            file.path = file.path.concat(String.fromCharCode(buffer[i]));
                        }
                        file.path = path.resolve(relative, file.path);
                        fs.readFile(file.path, (err, data) => {
                            if (err) {
                                file.content = "";
                            } else {
                                file.content = data.toString();
                            }
                            resolve();
                        });
                        file.cursor = 0;
                    });
                    break;
                case 3:
                    file.content = file.content.substring(0, file.cursor).concat(String.fromCharCode(registers[0])).concat(file.content.substring(file.cursor + 1));
                    file.cursor++;
                    break;
                case 4:
                    registers[0] = file.content.charCodeAt(file.cursor++);
                    break;
                case 5:
                    file.cursor = registers[2];
                    break;
                case 6:
                    registers[2] = file.cursor;
                    break;
                case 7:
                    registers[2] = file.content.length;
                    break;
                case 8:
                    await new Promise<void>(resolve => {
                        fs.writeFile(file.path, file.content, () => {
                            file.path = "";
                            file.content = "";
                            file.cursor = 0;
                            resolve();
                        });
                    });
                    break;
                case 9:
                    await new Promise<void>(resolve => {
                        setTimeout(resolve, registers[0]);
                    });
                    break;
                case 10:
                    cursor = buffer.length;
                    break;
            }
            cursor += 2;
        } else if (buffer[cursor] < 151) { // PUSH
            const arg0 = { type: buffer[cursor] - 151, value: buffer[cursor + 1] };
            stack.push(evaluate(arg0));
            cursor += 2;
        } else if (buffer[cursor] < 156) { // POP
            const arg0 = { type: buffer[cursor] - 156, value: buffer[cursor + 1] };
            insert(arg0, stack.pop());
            cursor += 2;
        } else if (buffer[cursor] < 157) { // HLT
            cursor++;
            process.exitCode = -1;
            throw new Error("치명적인♥ 에러");
        }
    }
}

async function bootstrap(file: string) {
    try {
        try {
            await fs.promises.access(file);
        } catch (e) {
            throw new Error(`${file} 몰?루`);
        }

        await run(await compile(await fs.promises.readFile(file, 'utf-8')), path.dirname(file));
    } catch (e) {
        process.stderr.write(`Error: ${e.message}\n`);
    }
}

if (process.argv[2]) {
    bootstrap(process.argv[2]);
}