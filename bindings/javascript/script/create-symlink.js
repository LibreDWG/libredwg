const fs = require("fs");
const path = require("path");

const args = process.argv.slice(2);

if (args.length < 2) {
  console.error("Usage: node create-symlink.js <source> <destination>");
  process.exit(1);
}

const source = path.resolve(args[0]);
const destination = path.resolve(args[1]);

// Remove existing symlink if it exists
if (fs.existsSync(destination)) {
  fs.unlinkSync(destination);
}

try {
  fs.symlinkSync(source, destination, "file");
  console.log(`Symbolic link created: ${destination} → ${source}`);
} catch (error) {
  console.error("Failed to create symlink:", error);
}