import { Baudrate } from './baudrate';

describe('Baudrate', () => {
  it('should create an instance', () => {
    expect(new Baudrate(115200)).toBeTruthy();
  });
});
