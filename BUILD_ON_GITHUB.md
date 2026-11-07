# Build the Windows VST3 without installing a compiler

This project includes a GitHub Actions workflow. GitHub's Windows build machine downloads JUCE, compiles the plugin, and publishes the `.vst3` bundle as a downloadable workflow artifact. Nothing needs to be compiled on your PC.

## 1. Create a GitHub repository

1. Sign in at https://github.com/.
2. Select **New repository**.
3. Name it `PulseVisualMetronome`.
4. Choose **Private** if you do not want the source code to be public. A public repository is also fine.
5. Create the repository.

## 2. Upload the project files

Extract the ZIP you received. In the new GitHub repository:

1. Select **Add file → Upload files**.
2. Upload `CMakeLists.txt`, `README.md`, `BUILD_ON_GITHUB.md`, and the `Source` folder with its four files.
3. Commit the upload to the `main` branch.

## 3. Add the build workflow

GitHub's normal file uploader can make hidden-folder uploads awkward, so create this workflow directly on GitHub:

1. Select **Add file → Create new file**.
2. Enter this exact filename: `.github/workflows/build-windows.yml`
3. Open the `build-windows.yml` file included in this ZIP in a text editor, copy all its contents, and paste them into GitHub's editor.
4. Select **Commit changes**.

The workflow should start automatically after the commit. You can also start it manually from the repository's **Actions** tab by selecting **Build Windows VST3 → Run workflow**.

## 4. Download the compiled plugin

1. Open the repository's **Actions** tab.
2. Select the latest **Build Windows VST3** run.
3. Wait until all steps show green check marks.
4. At the bottom of the run page, under **Artifacts**, download `Pulse-Visual-Metronome-Windows-VST3`.
5. Extract the downloaded ZIP. It contains the `Pulse Visual Metronome.vst3` bundle.

## 5. Install in Bitwig

Copy the complete `.vst3` folder (do not open it or copy only files from inside it) to:

`C:\Program Files\Common Files\VST3`

Restart or rescan plugins in Bitwig Studio.

## Troubleshooting

If the Actions run fails, open the failed run, select the red failed step, and copy the error text. Send it back so the build can be corrected. Do not download a failed or incomplete artifact.

GitHub-hosted runners provide the Windows compiler and build tools. The workflow fetches JUCE from its official GitHub repository during the build. No compiler, JUCE installation, or CMake installation is required on your computer.

## Important plugin behavior

This is the current first implementation. It is a visual metronome and transparent audio-through VST3; it does not generate audible metronome clicks. DAW synchronization uses timing information supplied by the host. Independent mode advances while the host processes the plugin, so the track must be active. Some advanced visualization features described in the project plan are not implemented yet.
